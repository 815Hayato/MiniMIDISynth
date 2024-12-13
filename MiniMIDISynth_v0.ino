#include <MIDI.h>
#include <Mozzi.h>
#include <mozzi_midi.h>
#include <Oscil.h>
#include <tables/square_no_alias_4096_int8.h>
#include <tables/saw4096_int8.h>
#include <LowPassFilter.h>

MIDI_CREATE_DEFAULT_INSTANCE();

#define CONTROL_RATE 64
#define NUM_PARTS 6 //声部(一度に演奏できるノートの数)
#define OSCIL_NUM_CELLS 4096 //基本波形データのデータ量

//波形データ(osc)・使用されているか(state)・音程(pitch)で構成される構造体
struct Part { 
  Oscil <OSCIL_NUM_CELLS, MOZZI_AUDIO_RATE> osc;
  bool state;
  byte pitch;
};

Part parts[NUM_PARTS];

LowPassFilter lpf;

//変数の設定
bool wave = true; //VCO: 基本波形(TRUE→SQUARE, FALSE→SAW)
bool vib_mode = false; //光センサを用いるか否か
int vib0; //光センサを修正する際に基準とする値
int threshold; //光センサを修正する際に基準とする値
int vib_modified; //修正後の光センサの値
byte gain; //音量

//noteONのMIDI信号を受信した際の動作
void handleNoteOn(byte channel, byte pitch, byte velocity){
  //空いているPart構造体にデータを代入
  for (int i=0;i<NUM_PARTS;i++){
    if(parts[i].state == false){
      parts[i].pitch = pitch;
      parts[i].state = true;
      break;
    }
  }
}

//noteOFFのMIDI信号を受信した際の動作
void handleNoteOff(byte channel, byte pitch, byte velocity){
  //Part構造体に代入していたデータを削除
  for (int i=0;i<NUM_PARTS;i++){
    if(parts[i].pitch == pitch){
      parts[i].state = false;
      break;
    }
  }
}

//初期設定
void setup() {
  //MIDI関連の設定
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.begin(1);

  //基本波形の設定
  for (int i=0;i<NUM_PARTS;i++){
    parts[i].osc.setTable(SQUARE_NO_ALIAS_4096_DATA);
    parts[i].state = false;
  }

  //mozziの設定
  startMozzi(CONTROL_RATE);
}

//繰り返し処理
void updateControl(){
  //変数の設定
  bool waveform = digitalRead(2); //VCO: TRUE→SQUARE, FALSE→SAW
  float ratio = 1.00; //VCO: 周波数にかける係数、あとで計算
  int cutoff = map(mozziAnalogRead(1),0,1023,0,255); //VCF: カットオフ周波数
  int resonance = map(mozziAnalogRead(2),0,1023,0,255); //VCF: レゾナンス
  gain = map(mozziAnalogRead(3),0,1023,0,255);
  bool vibOn = digitalRead(3); //光センサのON/OFF
  float vib = mozziAnalogRead(4); //光センサの値
  float pitch_delta = 0.00; //VCO: 周波数にかける係数(光センサ由来)、あとで計算

  //VCF: LPFを設定
  lpf.setCutoffFreqAndResonance(cutoff,resonance);
  
  //VCO: 基本波形の切り替え(SQUARE↔SAW)
  if(wave!=waveform){
    if(waveform == true){
      for(int i=0;i<NUM_PARTS;i++){
        parts[i].osc.setTable(SAW4096_DATA);
      }
    }else{
      for(int i=0;i<NUM_PARTS;i++){
        parts[i].osc.setTable(SQUARE_NO_ALIAS_4096_DATA);
      }
    }
    wave = waveform;
  }

  //VCO: 光センサにまつわる処理
  if(vibOn == true){ //光センサONからセンサの値を周波数に反映
    if(vib_mode == false){
      vib0 = vib;
      threshold = 1023 - 2*vib0;
      vib_mode = true;
    }
    
    if(vib > threshold){
      vib_modified = map(vib-vib0,vib0-1023,1023-vib0,255,-255);
    }else{
      vib_modified = -255;
    }
    pitch_delta = vib_modified/1536.00;
    ratio = pow(2,mozziAnalogRead(0)/1024.00 + vib_modified/1536.00);
  }else{ //光センサOFFなら反映しない
    if(vib_mode == true){
      vib_mode = false;
    }
    ratio = pow(2,mozziAnalogRead(0)/1024.00);
  }

  //VCO: 周波数の設定
  for(int i=0;i<NUM_PARTS;i++){
    parts[i].osc.setFreq(mtof(parts[i].pitch)*ratio);
  }
  
}

//音声出力処理
AudioOutput updateAudio(){
  int out = 0; //出力
  //各オシレータの波形を加算
  for (int i=0;i<NUM_PARTS;i++){
    if(parts[i].state == true){
      out += parts[i].osc.next();
    }
  }
  out = out >> 3; //÷8
  out = out * gain >> 8; //÷256

  //LPFをかけて出力
  return lpf.next(out);
}

void loop() {
  MIDI.read();
  audioHook();
}
