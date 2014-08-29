@extends('master')

@section('title')
	@parent - Prueba grabación audio
@stop

@section('styles')
<style>
	canvas { 
		display: inline-block; 
		background:#E0E0E0; 
		width: 95%;
		height: 250px;
		margin: 10px;
	}
</style>
@stop

@section('scripts')
	@parent
	{{ HTML::script('js/RecordRTC.js') }}
@stop
	

@section('scripts-jquery')

	<script>

	var recordRTC;

	$(document).ready(function(){
		
		navigator.getUserMedia({audio: true}, function(MediaStream) {
			/*if (window.IsChrome) MediaStream = new window.MediaStream(MediaStream.getAudioTracks());
			audioStream = MediaStream;*/
			
			recordRTC = RecordRTC(MediaStream);
			//gotStream(MediaStream);
			//loadStream(MediaStream);
		}, function(err){
			//("#rec").popover('show');
			//alert('Incompatible');
		});

		var recording = false;
		var blob;

		var sendAudio = function(action) {
			
		    function xhr(url, data, callback) {
		        var request = new XMLHttpRequest();
		        request.onreadystatechange = function () {
		            if (request.readyState == 4 && request.status == 200) {
			            if (action === "enroll") {
		                	alert("Enrollo!!");
			            } else {
		                	alert(request.responseText);
			            }
		            }
		        };
		        request.open('POST', url);
		        request.send(blob);
		    };

			xhr('{{ $apiurl }}/v1/voiceaccess/' + action + '/{{ $username }}', blob, function (fName) {
		        window.open(location.href + fName);
		    });
		};

		$("#rec").click(function() {
			if (recordRTC) {
				if (!recording) {
					$("#actionButtons").hide();
					recordRTC.startRecording();
					recording = true;
					$(this).html("{{Lang::get('speakerverification.stop_recording_button')}}");
				} else {
					recordRTC.stopRecording(function(audioURL) {
						blob = recordRTC.getBlob();
						//$('#player').attr('src',audioURL);
					});
					$("#actionButtons").show();
					recording = false;
					$(this).html("{{Lang::get('speakerverification.start_recording_button')}}");
				}
			} else {
				$("#rec").popover('show');
			}
		});

		$("#verify").click(function() {
			sendAudio('test');
		});

		$("#enroll").click(function() {
			sendAudio('enroll');
		});

	});

	</script>
@stop

@section('content_center')
	
	<div class="page-header">
		<h2>{{ Lang::get('speakerverification.title', array('username'=>$username)) }}</h2>
	</div>
	@if (!$enrolled)
		<p>@lang('speakerverification.not_enrolled_text')</p>
	@else
		<p>@lang('speakerverification.enrolled_text')</p>
	@endif
	
	<canvas id="analyser" width="1024" height="500"></canvas>
	
	<button id="rec" type="button" class="btn btn-default" data-container="body" data-toggle="popover" data-content="{{ Lang::get('speakerverification.allowaudio') }}">@lang('speakerverification.start_recording_button')</button>
	<div id="actionButtons" style="display: none">
		<button id="enroll" type="button" class="btn btn-primary btn-lg">@lang('speakerverification.enroll_button')</button>
		@if ($enrolled)
			<button id="verify" type="button" class="btn btn-primary btn-lg">@lang('speakerverification.verify_button')</button>
		@endif
	</div>
	
	<!-- <audio id="player" autoplay controls></audio>  -->

@stop