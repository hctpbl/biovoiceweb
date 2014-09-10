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
    {{-- HTML::script('https://www.google.com/jsapi') --}}
    
    <script>
    	/*google.load("visualization", "1", {packages:["corechart"]});
        google.setOnLoadCallback(drawChart);
        function drawChart(result, threshold) {
          var data = google.visualization.arrayToDataTable([
            ['a', 'b', { role: 'style' }],
            ["{{ Lang::get('speakerverification.verification_threshold') }}", threshold, 'blue'],
            ["{{ Lang::get('speakerverification.verification_result') }}", result, 'red']
          ]);

          var options = {
            title: "{{ Lang::get('verification_modal_title') }}"
          };

          var chart = new google.visualization.BarChart(document.getElementById('ver-modal-chart'));

          chart.draw(data, options);
        }*/
	</script>
@stop
	

@section('scripts-jquery')

	{{ HTML::script('js/jquery.runner-min.js') }}

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

		var startChrono = function() {
			chronometer.reset();
			//chronometer.start();
		}

		var sendAudio = function(action) {
			
		    function xhr(url, data, callback) {
            	var data, typeOfString;
		        var request = new XMLHttpRequest();
		        request.upload.addEventListener("progress", function(evt) {
			        if (evt.lengthComputable) {
				        var percentComplete = (evt.loaded / evt.total) * 100;
				        //$("#upload-progress").attr("value", percentComplete);
				        $("#upload-progress").css("width", percentComplete+"%");
				        $("#percentage-progress").html(parseInt(percentComplete, 10)+"%");
			        }
		        });
		        request.onreadystatechange = function () {
		            if (request.readyState == 4 && request.status == 200) {
						$("#showProgress").hide();
				        $("#upload-progress").css("width", "0%");
				        $("#percentage-progress").html("0%");
	                	data = JSON.parse(request.responseText);
			            if (action === "enroll") {
	                		if (!data.error) {
			            		$('#enrollmentModal').modal('show');
	                		} else {
			            		$('#enr-modal-text').html(data.message);
			            		$('#enrollmentModal').modal('show');
	                		};
			            } else {
	                		if (!data.error) {
		                		$('#ver-threshold-value').html(data.threshold + "");
		                		$('#ver-result-value').html(data.result + "");
		                		if (data.result >= data.threshold) {
			                		typeOfString = 'success';
			                		$('#ver-modal-subtitle').html("{{ Lang::get('speakerverification.verification_modal_subtitle_success') }}");
			                		$('#ver-modal-subtitle').css("color","green");
			                		$('#ver-modal-text').html("{{ Lang::get('speakerverification.verification_modal_text_success') }}");
		                		} else {
			                		$('#ver-modal-subtitle').html("{{ Lang::get('speakerverification.verification_modal_subtitle_failure') }}");
			                		$('#ver-modal-subtitle').css("color","red");
			                		$('#ver-modal-text').html("{{ Lang::get('speakerverification.verification_modal_text_failure') }}");
		                		};
			            		$('#verifyModal').modal('show');
	                		};
			            };
		            };
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
					$('#chrono').runner('reset');
				    $('#chrono').runner('start');
					$("#actionButtons").hide();
					$("#recording_img").show();
					recordRTC.startRecording();
					recording = true;
					$(this).html("{{Lang::get('speakerverification.stop_recording_button')}}");
					$(this).addClass("btn-danger");
					$(this).removeClass("btn-success");
				} else {
					recordRTC.stopRecording(function(audioURL) {
						blob = recordRTC.getBlob();
						console.log('Hey!');
						console.log(blob);
					});
				    $('#chrono').runner('stop');
					$("#actionButtons").show();
					$("#recording_img").hide();
					recording = false;
					$(this).html("{{Lang::get('speakerverification.start_recording_button')}}");
					$(this).addClass("btn-success");
					$(this).removeClass("btn-danger");
				}
			} else {
				$("#rec").popover('show');
			}
		});

		$("#verify").click(function() {
			$("#showProgress").show();
			sendAudio('test');
		});

		$("#enroll").click(function() {
			$("#showProgress").show();
			sendAudio('enroll');
		});

		$("#chrono").runner();

	});


	</script>

	{{ HTML::script('js/bootstrap.min.js') }}
@stop

@section('content_center')

	<div class="modal fade" id="enrollmentModal" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
	  <div class="modal-dialog">
	    <div class="modal-content">
	      <div class="modal-header">
	        <button type="button" class="close" data-dismiss="modal"><span aria-hidden="true">&times;</span><span class="sr-only">@lang('speakerverification.modal_close')</span></button>
	        <h4 class="modal-title">@lang('speakerverification.enrollment_modal_title')</h4>
	      </div>
	      <div class="modal-body">
	        <p id="enr-modal-text">@lang('speakerverification.enrollment_modal_text')</p>
	      </div>
	      <div class="modal-footer">
	        <button type="button" class="btn btn-default" data-dismiss="modal">@lang('speakerverification.modal_close')</button>
	      </div>
	    </div><!-- /.modal-content -->
	  </div><!-- /.modal-dialog -->
	</div><!-- /.modal -->

	<div class="modal fade" id="verifyModal" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
	  <div class="modal-dialog">
	    <div class="modal-content">
	      <div class="modal-header">
	        <button type="button" class="close" data-dismiss="modal"><span aria-hidden="true">&times;</span><span class="sr-only">@lang('speakerverification.modal_close')</span></button>
	        <h4 class="modal-title" id="ver-modal-title">@lang('speakerverification.verification_modal_title')</h4>
	      </div>
	      <div class="modal-body">
	        <h4 id="ver-modal-subtitle"></h4>
	        <p id="ver-modal-text"></p>
	        <ul>
	        	<li>@lang('speakerverification.verification_threshold'): <span id="ver-threshold-value"></span></li>
	        	<li>@lang('speakerverification.verification_result'): <span id="ver-result-value"></span></li>
	        </ul>
	        <div id="ver-modal-chart"></div>
	      </div>
	      <div class="modal-footer">
	        <button type="button" class="btn btn-default" data-dismiss="modal">@lang('speakerverification.modal_close')</button>
	      </div>
	    </div><!-- /.modal-content -->
	  </div><!-- /.modal-dialog -->
	</div><!-- /.modal -->
	
	<div class="page-header">
		<h2>{{ Lang::get('speakerverification.title', array('username'=>$username)) }}</h2>
	</div>
	@if (!$enrolled)
		<p>@lang('speakerverification.not_enrolled_text')</p>
	@else
		<p>@lang('speakerverification.enrolled_text')</p>
	@endif
	
	<canvas id="analyser" width="1024" height="500"></canvas>
	
	<button id="rec" type="button" class="btn btn-success" data-container="body" data-toggle="popover" data-content="{{ Lang::get('speakerverification.allowaudio') }}">@lang('speakerverification.start_recording_button')</button>
	<span style="width: 36px;">{{ HTML::image('img/ic_recording.png', null, array('style'=>'display: none', 'id'=>'recording_img')) }}</span>
	<span id="chrono"></span>
	<div id="actionButtons" style="display: none">
		<button id="enroll" type="button" class="btn btn-primary btn-lg"><span class="icon-mic"></span>@lang('speakerverification.enroll_button')</button>
		@if ($enrolled)
			<button id="verify" type="button" class="btn btn-primary btn-lg">@lang('speakerverification.verify_button')</button>
		@endif
	</div>
	<br />
	<div id="showProgress" style="display: none">
		<p>Uploading...</p>
		<div class="progress">
			<!-- <progress id="upload-progress" value="0" max="100"></progress>  -->
			<div id="upload-progress" class="progress-bar progress-bar-striped" role="progressbar" aria-valuenow="0" aria-valuemin="0" aria-valuemax="100" style="width: 0%">
			  <span id="percentage-progress"></span>
			</div>
		</div>
	</div>

@stop