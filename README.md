# MiniMIDISynth_v0
![MiniMIDISynth_v0](https://github.com/user-attachments/assets/ca41d628-a313-4b8b-9e27-aae0c005a708)

## 使用技術
Arduino,電気回路

## 概要
MIDIで動く小型シンセサイザー

## 使用例
以下のリンクからご覧いただけます。  
→https://drive.google.com/file/d/19zmoE3qn6c4s55bbvNtGxyEdIeQ93b18/view?usp=sharing

## 特徴
1. MIDIケーブルを通してMIDIコントローラー(キーボード・シーケンサーなど)を外付けして使用できる
2. VCO,VCF,VCAを搭載
3. 同時に最大6音を出力できる
4. 周囲の光量に応じてピッチを微妙に変動させることができる(光センサの利用)

## 機構
![MiniMIDISynth_v0_part](https://github.com/user-attachments/assets/8f0b8c1b-587a-4a26-9a91-205ace4a5418)
1. (VCO)波形切り替えスイッチ：基本波形をのこぎり波と矩形波で切り替える
2. (VCO)ピッチベンド：1oct.の幅で音程を滑らかに変化させる
3. (VCF)カットオフ周波数：ローパスフィルタのカットオフ周波数を設定する
4. (VCF)レゾナンス：ローパスフィルタのレゾナンスを設定する
5. (VCA)ゲイン：音量を調整する
6. (VCO)光センサ：光量に応じて周波数を変動させる
7. (VCO)光センサOnOffスイッチ：光センサの動作・休止を切り替える

## 改善点・今後の展望
1. 出力をステレオミニジャックにする
2. 基板化+小箱に収納して小型かつ持ち運べるようにする
3. モジュレーションやADSRなどの機能を追加する
4. 以前作成したElectronicSho_v0(https://github.com/815Hayato/ElectronicSho_v0 )をMIDIコントローラに改造した上で連動させる
