import("stdfaust.lib");

tapBeat = button("Tap Along");
kickGain = 1.27;
tempoMultiplier = 1;
snareGain = 1;
hhGain = 1.09;
crashGain = 0.2;
mel1Gain = 1.53;
mel2Gain = 1;
bassGain = 0.5;
bassIntenseGain = 1.05;
punishmentStrictness = 1;
relaxation = 0.3;
transpose = hslider("Transpose",0,-6,6,1);
stepCountSlider = hslider("Step Count",0,0,500,1);
tempoSlider = hslider("Set Tempo",110,20.0,150.0,0.01);
wet = hslider("Leslie Mix",1,0,1,0.01) : si.smoo;
kickBeatSel = hslider("[1] Kick Var",0,0,numKickTables-1,0.001);
snareBeatSel = hslider("[2]Snare Var",0,0,numSnareTables-1,0.001);
hhBeatSel = hslider("[3]HH Var",0,0,numHHTables-1,0.001);
playPause = checkbox("Play/Pause");

//TempoDetection///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tempo = tempoSlider*tempoMultiplier;

//StepCount

overall_stepCount = stepCountSlider;

///////////////////////////////////////////////////////////////////////////////// C L O C K S /////////////////////////////////////////////////////////////////////////////////////////////////

// Tempo to Samples
quarterNoteSampleTime = ba.tempo(tempo);

// Tempo to Seconds
sixteenthNoteTime = beatTime/4;
beatTime = 60/tempo; // Seconds per beat

// Clocks at Different Metrical Levels
masterClockBasic = ba.pulse(quarterNoteSampleTime/4);
swingAmp = 386;
swing = swingAmp * (0.5 + 0.5*no.noise);
swingDelay = swing : ba.sAndH(masterClockBasic);
masterClock = masterClockBasic : de.delay(400,swingDelay);
quarterClock = masterClock: ba.resetCtr(4,1);
eighthClock = masterClock: ba.resetCtr(2,1);
sixteenthClock = masterClock;

// Definition of a Measure: 16 Quarter Notes = 64 Sixteenth Notes = 4 BARS

//Pulse Counters
pulseCounter = _~+(1*masterClock);
beatCounter = (pulseCounter-1)/4 : int;
sixteenthCounterBar = (pulseCounter-1)%16;

barStart = sixteenthCounterBar%16 == 0;
melodyMeasureStart = (pulseCounter-1)%64 == 0;
percMeasureStart = (pulseCounter-1)%128 == 0;
barsElapsed = floor((pulseCounter-1)/16) : int;
melodyMeasuresElapsed = floor(barsElapsed/4) : int;
currentBar = barsElapsed%4 + 1;

measuresElapsed = floor((pulseCounter-1)/64) : int;

//Beat Counters

currentBeatCounter = _~+(quarterPulse) : _%(8);
firstBarCondition = (beatCounter+1)%16 <= 4;
secondBarCondition = (beatCounter+1)%16 <: <(9) & >(4);
thirdBarCondition = (beatCounter+1)%16 <: <(13) & >(8);
fourthBarCondition = (beatCounter+1)%16 > 12;

//////////////////////////////////////////////////////////////// E F F E C T S ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//FM Synth Filter

fmSynthFilter(fundamental_freq,mod1_index,mod2_index,mod3_index,mod1_ratio,mod2_ratio,mod3_ratio,env) = fmProduct : filterGain
  with
{
 fmProduct = env * os.osc(fundamental_freq+1*(env*mod1_index*os.osc(mod1Freq)
											            +env*mod2_index*os.osc(mod2Freq)
														+env*mod3_index*os.osc(mod3Freq)));
 mod1Freq = fundamental_freq*mod1_ratio;
 mod2Freq = fundamental_freq*mod2_ratio;
 mod3Freq = fundamental_freq*mod3_ratio;
 band1Gain = mod1_index/1000 * 5;
 band2Gain = mod2_index/1000 * 5;
 band3Gain = mod3_index/1000 * 5;
 attenuation_dB = (band1Gain + band2Gain + band3Gain) * -1;
 attenuation_lin = ba.db2linear(attenuation_dB);
 filterGain = _ <: fi.peak_eq_cq(band1Gain,mod1Freq,1) : fi.peak_eq_cq(band2Gain,mod2Freq,1) : fi.peak_eq_cq(band3Gain,mod3Freq,1) :> _*(attenuation_lin);
};

//FM Synth Dist

fmSynthDist(fundamental_freq,mod1_index,mod2_index,mod3_index,mod1_ratio,mod2_ratio,mod3_ratio,env) = fmProduct : speaker : dist : gain
  with
{
 fmProduct = env * os.osc(fundamental_freq+1*(env*mod1_index*os.osc(mod1Freq)
											            +env*mod2_index*os.osc(mod2Freq)
														+env*mod3_index*os.osc(mod3Freq)));
 mod1Freq = fundamental_freq*mod1_ratio;
 mod2Freq = fundamental_freq*mod2_ratio;
 mod3Freq = fundamental_freq*mod3_ratio;
 band1Gain = mod1_index/1000 * 10;
 band2Gain = mod2_index/1000 * 10;
 band3Gain = mod3_index/1000 * 10;
 attenuation_dB = (band1Gain + band2Gain + band3Gain) * -1;
 dist = ef.cubicnl((mod1_index+0.66*mod2_index+0.33*mod3_index)/3000,0);
 speaker = ef.speakerbp(40,15000);
 attenuation_lin = ba.db2linear(attenuation_dB);
 gain =  _*(attenuation_lin);
};

//Additive Synth

addSynth(f0) = oscillatorSum * 0.4
with
{
  oscillatorSum = os.osc(f0) + a2*os.osc(2*f0) + a3*os.osc(3*f0) + a4*os.osc(5*f0) + a5*os.osc(9*f0) + a6*os.osc(12*f0) + a7*os.osc(14*f0);
  a2 = no.noise : ba.sAndH(masterClock * barStart);
  a3 = no.noise : ba.sAndH(masterClock * barStart);
  a4 = no.noise : ba.sAndH(masterClock * barStart);
  a5 = no.noise : ba.sAndH(masterClock * barStart);
  a6 = no.noise : ba.sAndH(masterClock * barStart);
  a7 = no.noise : ba.sAndH(masterClock * barStart);
};

//Make Stereo

makeStereo = _ <: _,_;
panL = _,_ :_*(0.95),_*(0.1);
panR = _,_ :_*(0.4),_*(0.9); 

// Compressor-Limiter

compLimiter(inputGain_dB,ratio,thresh,att,rel,kneeAtt) = _*inputGain_lin 
  <: _*(an.amp_follower_ud(att,rel) : ba.linear2db : outminusindb : kneesmooth : ba.db2linear)
with{
  	inputGain_lin = ba.db2linear(inputGain_dB);
	outminusindb(level) = max(level-thresh,0)*(1/ratio-1);
	kneesmooth = si.smooth(ba.tau2pole(kneeAtt));
	//visualizer = hbargraph("[1]Compressor Level [unit:dB]",-50,10);
};

//Master Limiter
//limThresh = hslider("Limiter Threshold",0,72,-72,0.01);
limThresh = 0;
masterLimiter(ipGaindB) = _ : compLimiter(ipGaindB,10,limThresh,0.001,0.05,0.050);

//Haas Delay

haas = _ <: _, de.fdelay(10000,ma.SR * 0.015);

//Comb

comb(delayMs,feedback) = +~de.fdelay(10000,delSamples)*feedback
with
{
  delSamples = delayMs * 0.001 * ma.SR;
};

//Flanger

flanger(feedback,delay,rate,depth) = +~de.fdelay(10000,del)*feedback
with{
  del = (0.4*os.osc(rate) + 0.5)*delay*depth;
};

//Dotted Delay

dotted_delay(feedback,delay,wet,condition) = +~de.fdelay(100000,del)*feedback*wet*condition :>kickDrive
with{
  del = 0.75*delay;
};

// Hard Clipper Mono

hard_clip(limit) = _ : min(limit) : max(-1*limit) : _;

//Drive 

drive(gain,mix) = _ <:_,(_*(mix):ef.cubicnl(gain,0)):>_*(0.4);

//Ring Modulator

ringmod(depth) = _ <: _* (1 - depth) , _ * depth * os.osc(134) :> _*(1 + depth/2);

//Leslie
//wet = hslider("LeslieWet",1,0,1,0.01);
leslieWetDry = _ <: leslie*(0.5), _*(0.5) :> _;
modDepth = 1;
leslie = _ <: (_ : hpf : sdfTreble : amTreble), (_ : lpf : sdfBass : amBass) :> _ 
with
{
  leslieFreq = 6/beatTime;
  hpf = fi.highpass(4,800);
  lpf = fi.lowpass(4,800);
  sdfTreble = apfTreble : apfTreble : apfTreble : apfTreble;
  sdfBass = apfBass : apfBass : apfBass;  
  trebleMod = os.osc(leslieFreq + 0.1);
  bassMod = os.osc(1);
  trebleModSDF = -0.75 + 0.2 * modDepth * (trebleMod);
  bassModSDF = -0.92 + 0.04* modDepth * (bassMod);
  amTreble = _ * 0.95 + 0.05*trebleMod ;
  amBass = _ * 0.95 + 0.05*bassMod;
  apfTrebleNumCoeffs = trebleModSDF,1;
  apfTrebleDenCoeffs = trebleModSDF;
  apfBassNumCoeffs = bassModSDF,1;
  apfBassDenCoeffs = bassModSDF;
  apfTreble = fi.iir(apfTrebleNumCoeffs,apfTrebleDenCoeffs);
  apfBass = fi.iir(apfBassNumCoeffs,apfBassDenCoeffs);
};

// Modal Filter

modalFilterLF(excitation,freqsTable,gain,xFactor,Q) = 
excitation <: 
fi.resonbp(freqsTable(0)*xFactor,Q*1/xFactor,0.5 + xFactor),
fi.resonbp(freqsTable(1)*1/xFactor,Q*xFactor,0.5 - xFactor),
fi.resonbp(freqsTable(2)*xFactor,Q*1/xFactor,0.5 + xFactor),
fi.resonbp(freqsTable(3)*1/xFactor,Q*xFactor,0.5 - xFactor) :> _ * gain;

modalFilterHF(excitation,freqsTable,gain,xFactor,Q) = 
excitation <: 
fi.resonbp(freqsTable(4)*xFactor,Q*1/xFactor,0.5 + xFactor),
fi.resonbp(freqsTable(5)*1/xFactor,Q*xFactor,0.5 - xFactor),
fi.resonbp(freqsTable(6)*xFactor,Q*1/xFactor,0.5 + xFactor),
fi.resonbp(freqsTable(7)*1/xFactor,Q*xFactor,0.5 - xFactor),
fi.resonbp(freqsTable(8)*xFactor,Q*1/xFactor,0.5 + xFactor),
fi.resonbp(freqsTable(9)*1/xFactor,Q*xFactor,0.5 - xFactor):> _ * gain;

													 

//Reverb

reverb(feedback,apfGain) = _ <: comb(del1,feedback),comb(del2,feedback),comb(del3,feedback)
  							   ,comb(del4,feedback),comb(del5,feedback),comb(del6,feedback) :> (_: apf1 : apf1),(_ : apf1 : apf1)
with
{
  del1 = 50;
  del2 = 56;
  del3 = 61;
  del4 = 68;
  del5 = 72;
  del6 = 78;
  apf1numCoeffs = apfGain,1;
  apf1denCoeffs = apfGain;
  apf1 = fi.iir(apf1numCoeffs,apf1denCoeffs);
};

//Stereo Pair Selector

stereoFeedbackSelector(st_1,st_2,st_3,par_100to400) = L_output,R_output
with
{
  L_1 = st_1 : ba.selectn(2,0);
  R_1 = st_1 : ba.selectn(2,1);
  L_2 = st_2 : ba.selectn(2,0);
  R_2 = st_2 : ba.selectn(2,1);
  L_3 = st_3 : ba.selectn(2,0);
  R_3 = st_3 : ba.selectn(2,1);
  condition_1 = par_100to400 <: <(200) & >(99) : si.smooth(ba.tau2pole(0.5));
  condition_2 = par_100to400 <: <(300) & >(199) : si.smooth(ba.tau2pole(0.5));
  condition_3 = par_100to400 <: <(401) & >(299) : si.smooth(ba.tau2pole(0.5));
  L_output = L_1*condition_1 + L_2*condition_2 + L_3*condition_3;
  R_output = R_1*condition_1 + R_2*condition_2 + R_3*condition_3;
};

////////////////////////////////////////////////////////////////////// E N V E L O P E  G E N E R A T O R S ///////////////////////////////////////////////////////////////////////////////////

// Perc Envelope

percEnvelope(velocity1,velocity2,selFrac,attack,release) = hitGain * en.ar(attack,release,triggerCooked) 
  with
{
  hitGain = sqrt(0.5*(velocity1*(1-selFrac) + velocity2*selFrac));
  triggerCooked = 0.5*(velocity1 + velocity2) * masterClock;
};



///////////////////////////////////////////////////////////////P E R C - T A B L E S///////////////////////////////////////////////////////////////////////////////////////////////////////////

// Convention: Instrument_Type_Number where Type can be T = Trigger V = Velocity

percMainOsc = 0.5*(1 + no.noise);
kickSwitchOsc = numKickTables*percMainOsc;
snareSwitchOsc = numSnareTables*percMainOsc'''';
hhSwitchOsc = numHHTables * percMainOsc'';
kickSelector = kickSwitchOsc : ba.sAndH(percMeasureStart) : floor;
snareSelector = snareSwitchOsc : ba.sAndH(percMeasureStart) : floor;
hhSelector = hhSwitchOsc : ba.sAndH(percMeasureStart) : floor;

// KICK

numKickTables = 6;

// Kick Velocity

kick_V_1 = waveform{1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0};
kick_V_2 = waveform{1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0.8};
kick_V_3 = waveform{1,0,0,0,1,0,0,0.8,1,0,0,0,1,0,0,0.8};
kick_V_4 = waveform{1,0,0,1,0,0,1,0,0,0,1,0,0,0,1,0};
kick_V_5 = waveform{1,0,0,0, 0,0,0,1, 0,0,1,0,0,0,0,0};
kick_V_6 = waveform{1,0,0,1, 0,0,0,1, 0,1,1,0,0,0,1,0};

kick_V_1_Table(i) = kick_V_1,i : rdtable;
kick_V_2_Table(i) = kick_V_2,i : rdtable;
kick_V_3_Table(i) = kick_V_3,i : rdtable;
kick_V_4_Table(i) = kick_V_4,i : rdtable;
kick_V_5_Table(i) = kick_V_5,i : rdtable;
kick_V_6_Table(i) = kick_V_6,i : rdtable;

//																						Kick Table Selector
kickBeatSelInt = floor(kickSelector) : int;
kickBeatSelFrac = kickSelector - kickBeatSelInt;

kickVelTables = 
kick_V_1_Table(sixteenthCounterBar),
kick_V_2_Table(sixteenthCounterBar),
kick_V_3_Table(sixteenthCounterBar),
kick_V_4_Table(sixteenthCounterBar),
kick_V_5_Table(sixteenthCounterBar),
kick_V_6_Table(sixteenthCounterBar);
activeKickVelTable1 = kickVelTables : ba.selectn(numKickTables,kickBeatSelInt);
activeKickVelTable2 = kickVelTables : ba.selectn(numKickTables,min(kickBeatSelInt+1,numKickTables));

// SNARE

numSnareTables = 5;

// Snare Velocity

snare_V_1 = waveform{0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0};
snare_V_2 = waveform{0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1};
snare_V_3 = waveform{0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,1};
snare_V_4 = waveform{0,0,0,0,1,0,0,0,0,0,0,0,1,0.7,0,0.8};
snare_V_5 = waveform{0,0,0,0,1,0,0,1,0,0,0,0,1,0,0.9,1};

snare_V_1_Table(i) = snare_V_1,i : rdtable;
snare_V_2_Table(i) = snare_V_2,i : rdtable;
snare_V_3_Table(i) = snare_V_3,i : rdtable;
snare_V_4_Table(i) = snare_V_4,i : rdtable;
snare_V_5_Table(i) = snare_V_5,i : rdtable;

snareBeatSelInt = floor(snareSelector) : int;
snareBeatSelFrac = snareSelector - snareBeatSelInt;

snareVelTables = 
snare_V_1_Table(sixteenthCounterBar),
snare_V_2_Table(sixteenthCounterBar),
snare_V_3_Table(sixteenthCounterBar),
snare_V_4_Table(sixteenthCounterBar),
snare_V_5_Table(sixteenthCounterBar);
activeSnareVelTable1 = snareVelTables : ba.selectn(numSnareTables,snareBeatSelInt);
activeSnareVelTable2 = snareVelTables : ba.selectn(numSnareTables,min(snareBeatSelInt+1,numSnareTables));

// Hi Hat

numHHTables = 20;

// HH Velocity

HH_V_1 = waveform{0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0};
HH_V_2 = waveform{0,0,0.6,0.9,0,0,1,0,0,0,0.6,0.9,0,0,1,0};
HH_V_3 = waveform{0,0,0.7,1,0,0,0.7,1,0,0,0.7,1,0,0,0.7,1};
HH_V_4 = waveform{0,0,0.7,1,0,0.6,0.8,1,0,0,0.7,1,0,0.6,0.8,1};
HH_V_5 = waveform{1,0.6,0.35,0.6,1,0.6,0.35,0.6,1,0.6,0.35,0.6,1,0.6,0.35,0.6}; // ALL
HH_V_6 = waveform{0,1,1,1,0,0,1,0,0,1,1,1,0,1,0.6,0.8};
HH_V_7 = waveform{0,0.7,0.8,1,0,0,1,0,0.7,1,0,1,0,1,0,0.8};
HH_V_8 = waveform{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};
HH_V_9 = waveform{1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1};
HH_V_10 = waveform{1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,0};
HH_V_11 = waveform{1,1,0,1,1,1,0,1,1,1,0,1,1,0,1,0};
HH_V_12 = waveform{1,0,1,0,1,1,0,1,1,0,1,0,1,1,0,1};
HH_V_13 = waveform{1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,0};
HH_V_14 = waveform{1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0};
HH_V_15 = waveform{1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,1};
HH_V_16 = waveform{0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1};
HH_V_17 = waveform{1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0};
HH_V_18 = waveform{1,0,0,1,0,0,1,1,1,0,1,0,1,0,1,0};
HH_V_19 = waveform{1,0,0,1,1,0,1,0,1,1,0,1,1,0,1,1};
HH_V_20 = waveform{1,0,0,1,1,0,1,0,1,0,0,1,1,0,1,0};


HH_V_1_Table(i) = HH_V_1,i : rdtable;
HH_V_2_Table(i) = HH_V_2,i : rdtable;
HH_V_3_Table(i) = HH_V_3,i : rdtable;
HH_V_4_Table(i) = HH_V_4,i : rdtable;
HH_V_5_Table(i) = HH_V_5,i : rdtable;
HH_V_6_Table(i) = HH_V_6,i : rdtable;
HH_V_7_Table(i) = HH_V_7,i : rdtable;
HH_V_8_Table(i) = HH_V_8,i : rdtable;
HH_V_9_Table(i) = HH_V_9,i : rdtable;
HH_V_10_Table(i) = HH_V_10,i : rdtable;
HH_V_11_Table(i) = HH_V_11,i : rdtable;
HH_V_12_Table(i) = HH_V_12,i : rdtable;
HH_V_13_Table(i) = HH_V_13,i : rdtable;
HH_V_14_Table(i) = HH_V_14,i : rdtable;
HH_V_15_Table(i) = HH_V_15,i : rdtable;
HH_V_16_Table(i) = HH_V_16,i : rdtable;
HH_V_17_Table(i) = HH_V_17,i : rdtable;
HH_V_18_Table(i) = HH_V_18,i : rdtable;
HH_V_19_Table(i) = HH_V_19,i : rdtable;
HH_V_20_Table(i) = HH_V_20,i : rdtable;

hhBeatSelInt = floor(hhSelector) : int;
hhBeatSelFrac = hhSelector - hhBeatSelInt;

randomnessHHLevel = fourthBarCondition;
randomnessConditionHH = (pulseCounter%7 == 0); 
randomnessFactorHH = randomnessConditionHH * randomnessHHLevel;

hhVelTables = 
HH_V_1_Table(sixteenthCounterBar),
HH_V_2_Table(sixteenthCounterBar),
HH_V_3_Table(sixteenthCounterBar),
HH_V_4_Table(sixteenthCounterBar),
HH_V_5_Table(sixteenthCounterBar),
HH_V_6_Table(sixteenthCounterBar),
HH_V_7_Table(sixteenthCounterBar),
HH_V_8_Table(sixteenthCounterBar),
HH_V_9_Table(sixteenthCounterBar),
HH_V_10_Table(sixteenthCounterBar),
HH_V_11_Table(sixteenthCounterBar),
HH_V_12_Table(sixteenthCounterBar),
HH_V_13_Table(sixteenthCounterBar),
HH_V_14_Table(sixteenthCounterBar),
HH_V_15_Table(sixteenthCounterBar),
HH_V_16_Table(sixteenthCounterBar),
HH_V_17_Table(sixteenthCounterBar),
HH_V_18_Table(sixteenthCounterBar),
HH_V_19_Table(sixteenthCounterBar),
HH_V_20_Table(sixteenthCounterBar);
activeHHVelTable1 = hhVelTables : ba.selectn(numHHTables,hhBeatSelInt) : +(randomnessFactorHH);
activeHHVelTable2 = hhVelTables : ba.selectn(numHHTables,min(hhBeatSelInt+1,numHHTables)) : +(randomnessFactorHH);

//////////////////////////////////////////////////////////////////////////////////////Melody Synthesis/////////////////////////////////////////////////////////////////////////////////////////

numScales = 6;
major_scale = waveform{-1,0,2,4,5,7,9,11,12,14,16,17}; //Length = 12
minor_scale = waveform{-2,0,2,3,5,7,8,10,12,14,15,17,19,20,22,24};
minor_scale_var2 = waveform{-2,0,2,7,5,3,10,8,12,17,15,14,19,20,22,24};
phrygian_scale = waveform{-2,0,1,3,5,7,8,10,12,13,15,17};
arabic_scale = waveform{-2,0,1,4,5,7,8,10,12,13,16,17,19,20,22,24};
pentatonic_scale = waveform{-2,0,2,3,5,6,7,10,12,14,15,17,19,22,24,26};

majorScaleTable(i) = major_scale,i : rdtable;
minorScaleTable(i) = minor_scale,i : rdtable;
minorScaleVar2Table(i) = minor_scale_var2,i : rdtable;
phrygianScaleTable(i) = phrygian_scale,i : rdtable;
arabicScaleTable(i) = arabic_scale,i : rdtable;
pentatonicScaleTable(i) = pentatonic_scale,i : rdtable;

tonic = 36;
bassOctave = 0;
mel1Octave = 1;
mel2Octave = 2;

activeScale = 2;
//activeScale = 1;

randomSeedSlider = hslider("Random Seed",50,0,3000,1);
randomSeed(n) = de.delay(80000,randomSeedSlider*n);
melodyMainOsc = 0.5*(1 + no.noise) : randomSeed(0);
melodyPatternOsc = 0.5*(1 + no.noise) : randomSeed(7);
melodyAccOsc = 0.5*(1 + no.noise) : randomSeed(15);
//melodySwitchOsc = 4*floor(melodyMeasuresElapsed/4) + melodyMainOsc;
melodySwitchOsc = numBassMelodyTables*melodyMainOsc;
melodyPatternSwitchOsc = numBassVelTables*melodyPatternOsc;
accSwitchOsc = numAccTables * melodyAccOsc;
bassMelodySelector = melodySwitchOsc : ba.sAndH(melodyMeasureStart) : floor : int;
bassRhythmPatternSelector = melodyPatternSwitchOsc : ba.sAndH(melodyMeasureStart) : floor : int;
accSelector = accSwitchOsc : ba.sAndH(melodyMeasureStart) : floor : int;

humanizeAmount = 0;
humanizer = no.noise : ba.sAndH(masterClock) * (0.1 + thirdBarCondition + fourthBarCondition);

melodyFundamentalFreq(activeScale,tonic,octave,activeNoteDegree) = ba.midikey2hz(finalMidiValue)
with
{
  scaleIntervals =
	majorScaleTable(activeNoteDegree),
	minorScaleTable(activeNoteDegree),
  	minorScaleVar2Table(activeNoteDegree),
    phrygianScaleTable(activeNoteDegree),
	arabicScaleTable(activeNoteDegree),
  	pentatonicScaleTable(activeNoteDegree);
  	activeInterval = scaleIntervals : ba.selectn(numScales,activeScale);
  finalMidiValue = tonic + 12*octave + activeInterval; 
};

melodyAmplitudeEnvelope(noteLength,attack,release,velocity) = velocity * en.ar(attack,releaseCooked,triggerCooked) + humanizer * humanizeAmount
with
{
  releaseCooked = release * noteLength;
  triggerCooked = velocity * masterClock;
};


/////////////////////////////////////////////////////////////////////////////////////Bassline/////////////////////////////////////////////////////////////////////////////////////////////////

numBassMelodyTables = 24;
numBassVelTables = 25;
numAccTables = 10;

bassTable1 = waveform{1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1};
bassTable2 = waveform{1,1,1,1, 1,1,1,1, 1,1,1,1, 0,0,0,0};
bassTable3 = waveform{1,1,1,1, 1,1,1,1, 0,0,0,0, 3,3,3,3};
bassTable4 = waveform{1,1,1,1, 3,3,3,3, 0,0,0,0, 5,7,9,10};

bassTable5 = waveform{1,1,1,3, 0,0,0,3, 1,1,1,3, 0,0,2,3};
bassTable6 = waveform{1,1,1,3, 0,3,2,3, 1,1,7,3, 0,0,2,3};
bassTable7 = waveform{1,1,5,3, 0,0,6,5, 1,1,5,3, 4,3,2,0};
bassTable8 = waveform{1,1,5,8, 0,2,4,7, 1,1,5,3, 4,3,2,9};

bassTable9 = waveform{1,1,3,3, 0,0,3,3, 1,1,5,5, 2,2,3,7};
bassTable10 = waveform{1,1,3,3, 0,0,2,3, 1,1,5,3, 2,2,3,7};
bassTable11 = waveform{1,1,3,2, 1,0,5,2, 1,3,5,3, 2,3,4,6};
bassTable12 = waveform{1,1,3,8, 0,0,4,7, 1,5,3,2, 3,4,5,2};

bassTable13 = waveform{8,1,1,1, 7,1,1,1, 6,1,1,1, 5,3,2,3};
bassTable14 = waveform{8,1,1,1, 7,1,1,6, 5,1,1,5, 4,1,2,3};
bassTable15 = waveform{8,1,1,7, 1,1,6,1, 4,1,1,3, 1,3,2,0};
bassTable16 = waveform{8,1,7,1, 6,1,7,1, 6,1,5,1, 4,3,4,2};

bassTable17 = waveform{1,5,6,1, 4,5,1,1, 3,3,4,4, 2,2,0,0};
bassTable18 = waveform{1,5,6,1, 4,5,1,1, 3,4,5,3, 2,3,4,0};
bassTable19 = waveform{1,5,6,1, 4,5,1,1, 4,3,2,7, 5,6,5,3};
bassTable20 = waveform{1,5,6,1, 6,5,1,5, 4,1,3,1, 2,0,3,2};

bassTable21 = waveform{1,2,3,5, 0,2,4,7, 3,3,7,7, 2,2,7,7};
bassTable22 = waveform{1,1,3,3, 0,0,3,3, 1,1,5,5, 2,2,0,0};
bassTable23 = waveform{1,3,2,3, 0,3,2,3, 5,1,3,7, 4,2,0,2};
bassTable24 = waveform{1,1,3,1, 1,3,1,1, 5,1,3,1, 2,3,0,2};

bassMelody1(i) = bassTable1,i : rdtable;
bassMelody2(i) = bassTable2,i : rdtable;
bassMelody3(i) = bassTable3,i : rdtable;
bassMelody4(i) = bassTable4,i : rdtable;
bassMelody5(i) = bassTable5,i : rdtable;
bassMelody6(i) = bassTable6,i : rdtable;
bassMelody7(i) = bassTable7,i : rdtable;
bassMelody8(i) = bassTable8,i : rdtable;
bassMelody9(i) = bassTable9,i : rdtable;
bassMelody10(i) = bassTable10,i : rdtable;
bassMelody11(i) = bassTable11,i : rdtable;
bassMelody12(i) = bassTable12,i : rdtable;
bassMelody13(i) = bassTable13,i : rdtable;
bassMelody14(i) = bassTable14,i : rdtable;
bassMelody15(i) = bassTable15,i : rdtable;
bassMelody16(i) = bassTable16,i : rdtable;
bassMelody17(i) = bassTable17,i : rdtable;
bassMelody18(i) = bassTable18,i : rdtable;
bassMelody19(i) = bassTable19,i : rdtable;
bassMelody20(i) = bassTable20,i : rdtable;
bassMelody21(i) = bassTable21,i : rdtable;
bassMelody22(i) = bassTable22,i : rdtable;
bassMelody23(i) = bassTable23,i : rdtable;
bassMelody24(i) = bassTable24,i : rdtable;

bassVelTable1 = waveform{0.5,0,0.75,0.75, 0,0,0.75,0, 0,0,0.75,0.0, 0,0,0.75,0};
bassVelTable2 = waveform{0.75,0,0.75,0.75, 0,0,0.75,0, 0,0,0.75,0.75, 0,0.75,0,0.75};
bassVelTable3 = waveform{0.75,0.75,0,0.5, 0,0,0.75,0, 0,0,0.5,0.75, 0,0,0.75,0};
bassVelTable4 = waveform{0.75,0.0,0,0.6, 0.75,0,0.85,0, 0.5,0.5,0.75,0, 0.75,0.75,0.75,0};

bassVelTable5 = waveform{0.8,0.7,0.8,0, 0.5,0.5,0.85,0.65, 0.5,0.5,0.3,0.75, 0,0.45,0.75,0.5};
bassVelTable6 = waveform{0.5,0,0.8,0.6, 0.5,0.5,11.85,0, 0.5,0.5,0.3,0.75, 0,0.45,0.75,0.5};
bassVelTable7 = waveform{0.5,0.5,0.8,0.6, 0.5,0.5,0.85,0.65, 0.5,0.5,0.3,0.75, 0,0.45,0.75,0.5};
bassVelTable8 = waveform{0.5,0.5,0.8,0.6, 0.5,0.5,0.85,0.65, 0.5,0.5,0.3,0.75, 0,0.45,0.75,0.5};

bassVelTable9 = waveform{0.8,0.7,0.8,0, 0.8,0.7,0.8,0.0, 0.8,0.8,0.0,0.6, 0.8,0.0,0.7,0};
bassVelTable10 = waveform{0.8,0.7,0.8,0, 0.8,0.7,0,0.8, 0.8,0,0.6,0.6, 0.8,0.8,0.7,0};
bassVelTable11 = waveform{0.8,0.7,0.8,0, 0.8,0.7,0.7,0.8, 0,0.8,0.9,0.0, 0.8,0.8,0.7,0};
bassVelTable12 = waveform{0.8,0.7,0.8,0.8, 0,0.7,0,0.8, 0.8,0,0.6,0.6, 0.8,0.8,0.7,0};

bassVelTable13 = waveform{0.8,0.7,0.0,0.8, 0.8,0.0,0.8,0.7, 0.0,0.7,0.8,0.0, 0.5,0.85,0.75,0.0};
bassVelTable14 = waveform{0.8,0.7,0.0,0.8, 0.8,0.0,0.8,0.7, 0.8,0.7,0.8,0.0, 0.5,0.85,0.75,0.0};
bassVelTable15 = waveform{0.8,0.7,0.0,0.8, 0.8,0.8,0.,0.7, 0.0,0.7,0.8,0.0, 0.5,0,0.75,0.80};
bassVelTable16 = waveform{0.8,0.7,0.0,0.8, 0.8,0.8,0,0.7, 0.0,0.7,0.8,0.0, 0.5,0.85,0,0.8};

bassVelTable17 = waveform{0.5,0.5,0.8,0.6, 0.5,0.5,0.85,0.65, 0.5,0.5,0.3,0.75, 0,0.45,0.75,0.5};
bassVelTable18 = waveform{0.8,0.0,0.6,1, 0.0,0.6,0.1,0.0, 0.7,1,0.0,0.6, 0.8,0.0,0.7,0.7};
bassVelTable19 = waveform{0.8,0.0,0.6,0.8, 0.8,0.0,0.6,0.1, 0.7,1,0.0,0.6, 0.8,0.0,0.7,0.7};
bassVelTable20 = waveform{0.8,0.6,0.4,0.8, 0.6,0.4,0.8,0.6, 0.4,0.8,0.6,0.4, 0.5,0.5,0.8,0.6};

bassVelTable21 = waveform{0.6,0,0,0.8, 0.6,0,0.8,0, 0.9,0.3,0.7,0.3, 1,0.3,0.8,0};
bassVelTable22 = waveform{0.6,0,0,0.8, 0.6,0.3,0.8,0, 0.9,0.7,0.5,0, 0,0.3,0.8,0};
bassVelTable23 = waveform{0.6,0,0,0.8, 0.6,0,0.8,0, 0.3,0.7,0.3,0.8, 1,0.3,0.8,0};
bassVelTable24 = waveform{0.6,0.3,0.3,0.8, 0.3,0.8,0.9,0, 0.9,0.3,0.7,0.3, 1,0.3,0.8,0};

bassVelTable25 = waveform{1,0.4,1,0.4, 1,0.6,1,0.6, 0.8,0.3,0.8,0.7, 0.7,0.6,0.8,0};

bassVel1(i) = bassVelTable1,i : rdtable;
bassVel2(i) = bassVelTable2,i : rdtable;
bassVel3(i) = bassVelTable3,i : rdtable;
bassVel4(i) = bassVelTable4,i : rdtable;
bassVel5(i) = bassVelTable5,i : rdtable;
bassVel6(i) = bassVelTable6,i : rdtable;
bassVel7(i) = bassVelTable7,i : rdtable;
bassVel8(i) = bassVelTable8,i : rdtable;
bassVel9(i) = bassVelTable9,i : rdtable;
bassVel10(i) = bassVelTable10,i : rdtable;
bassVel11(i) = bassVelTable11,i : rdtable;
bassVel12(i) = bassVelTable12,i : rdtable;
bassVel13(i) = bassVelTable13,i : rdtable;
bassVel14(i) = bassVelTable14,i : rdtable;
bassVel15(i) = bassVelTable15,i : rdtable;
bassVel16(i) = bassVelTable16,i : rdtable;
bassVel17(i) = bassVelTable17,i : rdtable;
bassVel18(i) = bassVelTable18,i : rdtable;
bassVel19(i) = bassVelTable19,i : rdtable;
bassVel20(i) = bassVelTable20,i : rdtable;
bassVel21(i) = bassVelTable21,i : rdtable;
bassVel22(i) = bassVelTable22,i : rdtable;
bassVel23(i) = bassVelTable23,i : rdtable;
bassVel24(i) = bassVelTable24,i : rdtable;
bassVel25(i) = bassVelTable25,i : rdtable;

basslineRandomness = pulseCounter%measuresElapsed == 0;

bassMelodyTables = 
bassMelody1(sixteenthCounterBar),
bassMelody2(sixteenthCounterBar),
bassMelody3(sixteenthCounterBar),
bassMelody4(sixteenthCounterBar),
bassMelody5(sixteenthCounterBar),
bassMelody6(sixteenthCounterBar),
bassMelody7(sixteenthCounterBar),
bassMelody8(sixteenthCounterBar),
bassMelody9(sixteenthCounterBar),
bassMelody10(sixteenthCounterBar),
bassMelody11(sixteenthCounterBar),
bassMelody12(sixteenthCounterBar),
bassMelody13(sixteenthCounterBar),
bassMelody14(sixteenthCounterBar),
bassMelody15(sixteenthCounterBar),
bassMelody16(sixteenthCounterBar),
bassMelody17(sixteenthCounterBar),
bassMelody18(sixteenthCounterBar),
bassMelody19(sixteenthCounterBar),
bassMelody20(sixteenthCounterBar),
bassMelody21(sixteenthCounterBar),
bassMelody22(sixteenthCounterBar),
bassMelody23(sixteenthCounterBar),
bassMelody24(sixteenthCounterBar);

bassVelocityTables = 
bassVel1(sixteenthCounterBar),
bassVel2(sixteenthCounterBar),
bassVel3(sixteenthCounterBar),
bassVel4(sixteenthCounterBar),
bassVel5(sixteenthCounterBar),
bassVel6(sixteenthCounterBar),
bassVel7(sixteenthCounterBar),
bassVel8(sixteenthCounterBar),
bassVel9(sixteenthCounterBar),
bassVel10(sixteenthCounterBar),
bassVel11(sixteenthCounterBar),
bassVel12(sixteenthCounterBar),
bassVel13(sixteenthCounterBar),
bassVel14(sixteenthCounterBar),
bassVel15(sixteenthCounterBar),
bassVel16(sixteenthCounterBar),
bassVel17(sixteenthCounterBar),
bassVel18(sixteenthCounterBar),
bassVel19(sixteenthCounterBar),
bassVel20(sixteenthCounterBar),
bassVel21(sixteenthCounterBar),
bassVel22(sixteenthCounterBar),
bassVel23(sixteenthCounterBar),
bassVel24(sixteenthCounterBar),
bassVel25(sixteenthCounterBar);


BassActiveNote = bassMelodyTables : ba.selectn(numBassMelodyTables,bassMelodySelector);
BassActiveVelocity = bassVelocityTables : ba.selectn(numBassVelTables,bassRhythmPatternSelector);
BassActiveNoteDegree = BassActiveNote;
BassActiveNoteDegreeNormalized = BassActiveNoteDegree%11;
BassActiveNoteVelocity = 1.3*(BassActiveVelocity%11);
BassActiveNoteLength = 1 + floor(BassActiveVelocity/11);

///////////////////////////////////////////////////////////////////////////////////////Melody (Accentuation)//////////////////////////////////////////////////////////////////////////////////

accTable1 = waveform{1,0,0,1, 0,0,1,0, 0,1,1,0, 1,0,1,1};
accTable2 = waveform{1,0,1,1, 1,0,1,1, 1,1,0,1, 1,0,1,0};
accTable3 = waveform{1,1,0,1, 1,0,1,0, 1,0,1,0, 1,0,1,0};
accTable4 = waveform{1,0,0,1, 0,0,1,1, 0,1,1,0, 1,1,1,1};
accTable5 = waveform{1,1,0,1, 1,0,1,1, 0,1,1,0, 1,0,1,0};
accTable6 = waveform{1,0,0,1, 1,0,1,1, 1,0,1,0, 1,1,1,1};
accTable7 = waveform{1,1,0,1, 1,1,0,1, 0,1,1,0, 1,1,0,1};
accTable8 = waveform{1,0,0,1, 1,0,1,0, 0,1,1,1, 1,0,1,1};
accTable9 = waveform{1,1,1,1, 0,1,0,1, 0,1,1,0, 1,0,1,0};
accTable10 = waveform{1,0,1,1, 0,1,1,0, 1,1,0,1, 1,1,1,1};

accRead1(i) = accTable1,i : rdtable;
accRead2(i) = accTable2,i : rdtable;
accRead3(i) = accTable3,i : rdtable;
accRead4(i) = accTable4,i : rdtable;
accRead5(i) = accTable5,i : rdtable;
accRead6(i) = accTable6,i : rdtable;
accRead7(i) = accTable7,i : rdtable;
accRead8(i) = accTable8,i : rdtable;
accRead9(i) = accTable9,i : rdtable;
accRead10(i) = accTable10,i : rdtable;

accentTables = 
accRead1(sixteenthCounterBar),
accRead2(sixteenthCounterBar),
accRead3(sixteenthCounterBar),
accRead4(sixteenthCounterBar),
accRead5(sixteenthCounterBar),
accRead6(sixteenthCounterBar),
accRead7(sixteenthCounterBar),
accRead8(sixteenthCounterBar),
accRead9(sixteenthCounterBar),
accRead10(sixteenthCounterBar);

activeAccent = accentTables : ba.selectn(numAccTables,accSelector);

mel1ActiveNote = bassMelodyTables : ba.selectn(numBassMelodyTables,bassMelodySelector);
mel1ActiveVelocity = bassVelocityTables : ba.selectn(numBassVelTables,bassRhythmPatternSelector);
mel1ActiveNoteDegree = mel1ActiveNote + 2*secondBarCondition + 4*fourthBarCondition;
mel1ActiveNoteDegreeNormalized = mel1ActiveNoteDegree%11;
mel1ActiveNoteVelocity = 1.3*(mel1ActiveVelocity%11);
mel1ActiveNoteLength = 1 + floor(mel1ActiveVelocity/11);

//////////////////////////////////////////////////////////////////////////////////////Individual Synthesis/////////////////////////////////////////////////////////////////////////////////////

//==========================================================================================K I C K============================================================================================

//Envelope Generation
kickEnvelopeLF = percEnvelope(activeKickVelTable1,activeKickVelTable2,kickBeatSelFrac,0.00001,kickRelease);
kickEnvelopeLF2 = percEnvelope(activeKickVelTable1,activeKickVelTable2,kickBeatSelFrac,0.00001,kickRelease);
kickEnvelopeHF = percEnvelope(activeKickVelTable1,activeKickVelTable2,kickBeatSelFrac,0.0001,0.03);

																						// Kick Track 1

//Trigger																					
lowDjembeTrigger = kickEnvelopeLF;

//Synthesis
lowDjembe = lowDjembeTrigger : pm.djembe(ba.midikey2hz(tonic+12),1,1,1) * 5 : fi.peak_eq_cq(4,2500,0.7);

//Signal Chain
kickTrack1 = lowDjembe * 3 : makeStereo;

																						// Kick Track 2

kickSawMid = os.osc(kickfreq*2*kickEnvelopeLF2);
kickExcitation1_Mid = kickSawMid*kickEnvelopeLF2 : kickFilter*2;
kickExcitationMid = kickExcitation1_Mid,kickExcitation2 * (0.5) :> _;

kickTrack2 = kickExcitationMid : kickDrive * kickGain * 7 : makeStereo;
																						// Kick Track 3

//Initialize
walkingParamKick = 0.4;
kickfreq = 177.2 + 73*walkingParamKick;
kickRelease = (0.096 + 0.104*walkingParamKick)*110/tempo;
kickBeaterLevel = 0.35;

//Excitation
kickSaw = os.osc(kickfreq*kickEnvelopeLF);
kickNoise  = no.noise :fi.bandpass(2,1500,5200)*kickBeaterLevel;
kickExcitation1 = kickSaw*kickEnvelopeLF : kickFilter*2;
kickExcitation2 = kickNoise * kickEnvelopeHF;
kickExcitation = kickExcitation1,kickExcitation2 :> _;

//Processing
kickFilter = fi.resonlp(8000,1,1) : fi.peak_eq_cq(8,60,0.7);
kickDrive = _ <:_,(_*(0.01):ef.cubicnl(0.6,0)):>_*(0.4);

//Signal Chain
kickTrack3 = kickExcitation : kickDrive * kickGain * 7 : makeStereo;

//==========================================================================================S N A R E==========================================================================================

//Envelope Generation
snareEnvelopeLF = pow(percEnvelope(activeSnareVelTable1,activeSnareVelTable2,snareBeatSelFrac,0.0001,snareReleaseLF),2);
snareEnvelopeHF = pow(percEnvelope(activeSnareVelTable1,activeSnareVelTable2,snareBeatSelFrac,0.0001,snareReleaseHF),2);

																						// Snare Track 1

//Trigger
highDjembeTrigger = snareEnvelopeLF;

//Synthesis
highDjembeTrack = highDjembeTrigger : pm.djembe(ba.midikey2hz(tonic+19),1,1,1) : fi.peak_eq_cq(4,2500,0.7);

//Signal Chain
snareTrack1 = highDjembeTrack * 5 : makeStereo : panR;

																						// Snare Track 2
snare_BPF_centerFreqs2 = waveform{130,306,504,585,672,816,1021,1307,1504,1960};
snare_centerFreqs2(i) = snare_BPF_centerFreqs2,i : rdtable;

snareFilteredLF2 = modalFilterLF(snareExcitationLF,snare_centerFreqs2,snareLFGain,snareModalXFactor,snareModalQ*1.5);
snareFilteredHF2 = modalFilterHF(snareExcitationHF,snare_centerFreqs2,snareHFGain,snareModalXFactor,snareModalQ*1.5);
snareCombined2 = snareFilteredHF2 + snareFilteredLF2*2;

//Signal Chain
snareTrack2 = snareCombined2 : _ * snareGain * 3 : makeStereo;

																						// Snare Track 3

//Initialize
walkingParamSnare = 0;
snare_BPF_centerFreqs = waveform{165,276,394,585,1572,2116,3021,3907,5804,7360};
snare_centerFreqs(i) = snare_BPF_centerFreqs,i : rdtable;
snareReleaseLF = 0.292;
snareReleaseHF = 0.265;
snareModalQ = 1.373 + walkingParamSnare*5.455;
snareModalXFactor = 1.107 - walkingParamSnare*0.43;
snareLFGain = 1.373 + walkingParamSnare*1.92;
snareHFGain = 1.683;

//Excitations
noiseSourceSnare = no.noise;
snareExcitationLF = noiseSourceSnare*snareEnvelopeLF*snareEnvelopeLF;
snareExcitationHF = noiseSourceSnare*snareEnvelopeHF*snareEnvelopeHF;

//Synthesis
snareFilteredLF = modalFilterLF(snareExcitationLF,snare_centerFreqs,snareLFGain,snareModalXFactor,snareModalQ);
snareFilteredHF = modalFilterHF(snareExcitationHF,snare_centerFreqs,snareHFGain,snareModalXFactor,snareModalQ);
snareCombined = snareFilteredHF + snareFilteredLF*2;

//Signal Chain
snareTrack3 = snareCombined : _ * snareGain * 3 : makeStereo;

//==========================================================================================H I H A T==========================================================================================

hhEnvelope = pow(percEnvelope(activeHHVelTable1,activeHHVelTable2,hhBeatSelFrac,0.0001,hhRelease),2);

																						//HH Track 1
//Synthesis
hhMarimba = hhEnvelope : pm.marimba(800,0.75,5000,hhEnvelope,1) * 5;

//Signal Chain
hhTrack1 = hhMarimba * 2 : makeStereo : panL;

																						//HH Track 2

hhTrack2 = hhBaseSignal * hhEnvelope : fi.bandpass(2,5000,10000) * 5 : makeStereo : panL;

																						//HH Track 3

//Initialize
hhRelease = 0.1 * (1+randomnessConditionHH*randomnessHHLevel+abs(humanizer)*0.6);
hhFilter = fi.bandpass(2,10000,20000) ;

//Excitations
hhBaseSignal = no.noise;
hhExcitation = hhBaseSignal*hhEnvelope : hhFilter;

//Signal Chain
hhTrack3 = hhExcitation * 5 : makeStereo : panL;

//==========================================================================================B A S S============================================================================================

//Fundamental Frequency Determination
bassline_fundamental_freq = melodyFundamentalFreq(activeScale,tonic,bassOctave,BassActiveNoteDegreeNormalized) : si.smooth(ba.tau2pole(bass_portamento));
bass_portamento = secondBarCondition,thirdBarCondition,fourthBarCondition : select3(1+barsElapsed%3) * 0.025;

//Envelope Generation
bassline_envelope = pow(melodyAmplitudeEnvelope(BassActiveNoteLength,0.001,bassRelease,BassActiveNoteVelocity),2) : si.smooth(ba.tau2pole(0.005));

//Parameters
bassGain_dB = 8.5;
bassRelease = 0.25;
basslineExcitement = 70;

//Flanger Initialization
bassFilter = fi.resonhp(80,0.707,1) : fi.peak_eq_cq(12,120,0.5) ;
bassFlangerControl = 0.7 * fourthBarCondition;
bassFlanger = flanger(bassFlangerControl,440,tempo/480,bassFlangerControl);


																						//Bass Track 1

//Excitation + Synthesis
bassExcitationSoft = os.osc(bassline_fundamental_freq*2) * bassline_envelope;

//Signal Chain
bassTrack1 = bassExcitationSoft : bassFilter : bassFlanger : makeStereo;

																						//Bass Track 2

basslineExcitement2 = 35;
bassExcitationMiddle = fmSynthFilter(bassline_fundamental_freq*2,8*basslineExcitement2,4*basslineExcitement2,1*basslineExcitement2,6,11,19,bassline_envelope) : _ * pow(10,bassGain_dB/20);
bassTrack2 = bassExcitationMiddle : fi.peak_eq_cq(-3,200,1) : bassFilter : bassFlanger : makeStereo;

																						//Bass Track 3

basslineExcitement3 = 65;

//Synthesis
bassExcitationHard = fmSynthDist(bassline_fundamental_freq,8*basslineExcitement3,4*basslineExcitement3,1*basslineExcitement3,4,7,19,bassline_envelope) : _ * pow(10,bassGain_dB/20);

//Signal Chain
bassTrack3 = bassExcitationHard : bassFilter : bassFlanger : makeStereo;

//==========================================================================================M E L O============================================================================================

//Fundamental Frequency Determination
mel1_fundamental_freq = melodyFundamentalFreq(activeScale,tonic,mel2Octave,mel1ActiveNoteDegreeNormalized);

//Envelope Generation
mel1_envelope = pow(melodyAmplitudeEnvelope(mel1ActiveNoteLength,0.001,mel1Release,mel1ActiveNoteVelocity),2) * activeAccent : si.smooth(ba.tau2pole(0.001));

//Parameters
mel1Gain_dB = 22;
mel1Release = 0.35 * tempo/110;

																						//Melody Track 1

//Excitation + Synthesis
mel1ExcitationSoft = os.osc(mel1_fundamental_freq) * mel1_envelope : _ * pow(10,mel1Gain_dB/20)/(1 + humanizeAmount) * 0.55;

//Signal Chain
melodyTrack1 = mel1ExcitationSoft : haas;

																						//Melody Track 2

//melExcitationMiddle = os.triangle(mel1_fundamental_freq) * mel1_envelope : _ * pow(10,mel1Gain_dB/20) /(1 + humanizeAmount); 

melodyTrack2 = mel1ExcitationHard : dotted_delay(0.8,quarterNoteSampleTime,0.4,1) : fi.resonlp(1000,2,1) : haas;

																						//Melody Track 3

//Excitation + Synthesis
mel1ExcitationHard = os.triangle(mel1_fundamental_freq) * mel1_envelope : _ * pow(10,mel1Gain_dB/20) /(1 + humanizeAmount) * 0.8;

//Signal Chain
melodyTrack3 = mel1ExcitationHard : dotted_delay(0.8,quarterNoteSampleTime,0.4,1) : fi.peak_eq_cq(3.5,250,0.7) : fi.resonlp(5000,2,1) * 1.3 : leslieWetDry : haas;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Define External Feedback Sliders
audioParam1 = 100 + 100 * (allInstrumentState-1) + 100 * (percInstrumentState-1);
audioParam2 = 100 + 100 * (allInstrumentState-1) + 100 * (percInstrumentState-1);
audioParam3 = 100 + 100 * (allInstrumentState-1) + 100 * (percInstrumentState-1);
audioParam4 = 100 + 100 * (allInstrumentState-1) + 100 * (melInstrumentState-1);
audioParam5 = 100 + 100 * (allInstrumentState-1) + 100 * (melInstrumentState-1);

//Define Global Changes
allInstrumentState = hslider("Global Energy State",3,1,3,1);
percInstrumentState = hslider("Perc Energy State",1,1,3,1);
melInstrumentState = hslider("Melody Energy State",1,1,3,1);

beepTrigger = (allInstrumentState != allInstrumentState') + (percInstrumentState != percInstrumentState') + (melInstrumentState != melInstrumentState') + (beeper != beeper');
beeper = checkbox("Beep");

beepTrack = os.osc(3000) * en.ar(0.001,0.2,beepTrigger) * en.ar(0.001,0.2,beepTrigger) * 4 : makeStereo;

///////////////////////////////////////////////////////////////////////////////////////Group Submix////////////////////////////////////////////////////////////////////////////////////////////

kickTrack = stereoFeedbackSelector(kickTrack1,kickTrack2,kickTrack3,audioParam1);
snareTrack = stereoFeedbackSelector(snareTrack1,snareTrack2,snareTrack3,audioParam2);
hhTrack = stereoFeedbackSelector(hhTrack1,hhTrack2,hhTrack3,audioParam3);
bassTrack = stereoFeedbackSelector(bassTrack1,bassTrack2,bassTrack3,audioParam4);
melodyTrack = stereoFeedbackSelector(melodyTrack1,melodyTrack2,melodyTrack3,audioParam5);

limiterInputGain = 0;
masterGain = _*(0.04),_*(0.04);
process = kickTrack,snareTrack,hhTrack,bassTrack,melodyTrack,beepTrack
		  :> masterGain : (_:masterLimiter(limiterInputGain): hard_clip(1)),(_:masterLimiter(limiterInputGain): hard_clip(1));
