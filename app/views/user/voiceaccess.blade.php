@extends('master')

@section('content_center')
	
	<div class="page-header">
		<h2>{{ Lang::get('voiceaccess.page-title') }}</h2>
	</div>
	<p>{{ Lang::get('voiceaccess.page-intro') }}</p>
	
	<div class="page-header">
		<h2>{{ Lang::get('voiceaccess.access-title') }}</h2>
	</div>
	<p>{{ Lang::get('voiceaccess.access-instructions') }}</p>
	<!-- <p>{{ Lang::get('voiceaccess.access-warning') }}</p>  -->
	
	{{ Form::open(array('action' => 'bvw\Controller\User@getVoiceaccess', 'method' => 'GET')) }}
	
		<div class="form-group @if($errors->has('username'))has-error@endif">
			{{ Form::label('username', Lang::get('user.id')) }}
			{{ $errors->first('username', '<div class="alert alert-danger">:message</div>') }}
			<div class="row">
				<div class="col-xs-3">
					{{ Form::text('username', Input::old('username'), array('class'=>'form-control', 'required'=>'')) }}
				</div>
			</div>
			<span class="help-block">{{ Lang::get('user.id-help') }}</span>
		</div>
		
		<button type="submit" class="btn btn-default">{{ Lang::get('user.register') }}</button>
	
	{{ Form::close() }}
	
	<div class="page-header">
		<h2>{{ Lang::get('voiceaccess.description-title') }}</h2>
	</div>
	<p>{{ Lang::get('voiceaccess.description-intro') }}</p>
	<ol>
		<li>{{ Lang::get('voiceaccess.description-step1') }}</li>
		<li>{{ Lang::get('voiceaccess.description-step2') }}</li>
	</ol>

@stop