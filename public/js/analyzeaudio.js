	window.AudioContext = window.AudioContext || window.webkitAudioContext;

    var context = new AudioContext();
    var audioBuffer;
    var sourceNode;
    var analyser;
    var javascriptNode;

    // get the context from the canvas to draw on
    var ctx = $("#analyser").get()[0].getContext("2d");

    // create a gradient for the fill. Note the strange
    // offset, since the gradient is calculated based on
    // the canvas, not the specific element we draw
    var gradient = ctx.createLinearGradient(0,0,0,300);
    gradient.addColorStop(1,'#000000');
    gradient.addColorStop(0.75,'#ff0000');
    gradient.addColorStop(0.25,'#ffff00');
    gradient.addColorStop(0,'#ffffff');


    // load the sound
    setupAudioNodes();


    function setupAudioNodes() {

        // setup a javascript node
        javascriptNode = context.createScriptProcessor(2048, 1, 1);
        // connect to destination, else it isn't called
        javascriptNode.connect(context.destination);


        // setup a analyzer
        analyser = context.createAnalyser();
        analyser.smoothingTimeConstant = 0.3;
        analyser.fftSize = 512;
    }
    
    function loadStream(stream) {

        // create a buffer source node
        sourceNode = context.createMediaStreamSource(stream);
        sourceNode.connect(analyser);
        analyser.connect(javascriptNode);

        sourceNode.connect(context.destination);
    }

    // log if an error occurs
    function onError(e) {
        console.log(e);
    }

    // when the javascript node is called
    // we use information from the analyzer node
    // to draw the volume
    javascriptNode.onaudioprocess = function() {

        // get the average for the first channel
        var array =  new Uint8Array(analyser.frequencyBinCount);
        analyser.getByteFrequencyData(array);

        // clear the current state
        ctx.clearRect(0, 0, 1000, 325);

        // set the fill style
        ctx.fillStyle=gradient;
        drawSpectrum(array);

    }


    function drawSpectrum(array) {
        for ( var i = 0; i < (array.length); i++ ){
            var value = array[i];

            ctx.fillRect(i*5,325-value,3,325);
            //  console.log([i,value])
        }
    };