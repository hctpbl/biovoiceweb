@extends('master')

@section('title')
	@parent - Prueba grabación audio
@stop

@section('scripts')
	@parent
	{{ HTML::script('js/RecordRTC.js') }}
	
	<script>

	$(document).ready(function(){
		
		navigator.getUserMedia({audio: true}, function(MediaStream) {
		   window.recordRTC = RecordRTC(MediaStream);
		});

		$("#rec").click(function() {
			recordRTC.startRecording();
		});

		$("#stop").click(function() {
		   recordRTC.stopRecording(function(audioURL) {
		      //window.open(audioURL);
		      var blob = recordRTC.getBlob();
		      
		      var fileType = 'audio';
		      var fileName = 'ABCDEF.wav';

		      var formData = new FormData();
		      formData.append(fileType + '-filename', fileName);
		      formData.append(fileType + '-blob', blob);

		      xhr('api/upload-audio', formData, function (fName) {
		          window.open(location.href + fName);
		      });

		      function xhr(url, data, callback) {
		          var request = new XMLHttpRequest();
		          request.onreadystatechange = function () {
		              if (request.readyState == 4 && request.status == 200) {
		                  callback(location.href + request.responseText);
		              }
		          };
		          request.open('POST', url);
		          request.send(data);
		      }
		   });
		});

	});

	</script>
@stop

@section('content_center')
	
	<div class="page-header">
		<h2>{{ Lang::get('speakerverification.title', array('username'=>$username)) }}</h2>
	</div>
	
	<button id="rec" type="button" class="btn btn-default">Record</button>
	<button id="stop" type="button" class="btn btn-default">Stop</button>

@stop