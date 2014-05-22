/* Copyright 2013 Chris Wilson

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

window.AudioContext = window.AudioContext || window.webkitAudioContext;

var audioContextDisp = new AudioContext();
var audioInputDisp = null,
    realAudioInputDisp = null,
    inputPointDisp = null;
var rafID = null;
var analyserContextDisp = null;
var canvasWidth, canvasHeight;

function updateAnalysers(time) {
    if (!analyserContextDisp) {
        var canvas = document.getElementById("analyser");
        canvasWidth = canvas.width;
        canvasHeight = canvas.height;
        analyserContextDisp = canvas.getContext('2d');
    }

    // analyzer draw code here
    {
        var SPACING = 3;
        var BAR_WIDTH = 1;
        var numBars = Math.round(canvasWidth / SPACING);
        var freqByteData = new Uint8Array(analyserNode.frequencyBinCount);

        analyserNode.getByteFrequencyData(freqByteData); 

        analyserContextDisp.clearRect(0, 0, canvasWidth, canvasHeight);
        analyserContextDisp.fillStyle = '#F6D565';
        analyserContextDisp.lineCap = 'round';
        var multiplier = analyserNode.frequencyBinCount / numBars;

        // Draw rectangle for each frequency bin.
        for (var i = 0; i < numBars; ++i) {
            var magnitude = 0;
            var offset = Math.floor( i * multiplier );
            // gotta sum/average the block, or we miss narrow-bandwidth spikes
            for (var j = 0; j< multiplier; j++)
                magnitude += freqByteData[offset + j];
            magnitude = magnitude / multiplier;
            //var magnitude2 = freqByteData[i * multiplier];
            analyserContextDisp.fillStyle = "hsl( " + Math.round((i*360)/numBars) + ", 100%, 50%)";
            analyserContextDisp.fillRect(i * SPACING, canvasHeight, BAR_WIDTH, -magnitude);
        }
    }
    
    rafID = window.requestAnimationFrame( updateAnalysers );
}

function gotStream(stream) {
    inputPointDisp = audioContextDisp.createGain();

    // Create an AudioNode from the stream.
    realAudioInputDisp = audioContextDisp.createMediaStreamSource(stream);
    audioInputDisp = realAudioInputDisp;
    audioInputDisp.connect(inputPointDisp);

    analyserNode = audioContextDisp.createAnalyser();
    //analyserNode.fftSize = 2048;
    inputPointDisp.connect( analyserNode );

    zeroGain = audioContextDisp.createGain();
    zeroGain.gain.value = 0.0;
    inputPointDisp.connect( zeroGain );
    zeroGain.connect( audioContextDisp.destination );
    updateAnalysers();
}
