@extends('master')

@section('content_center')
	
	<div class="page-header">
		<h2>{{ Lang::get('registersuccess.page-title') }}</h2>
	</div>
	<p>{{ Lang::get('registersuccess.page-intro') }}</p>
	
	<ul>
		<li>{{ Lang::get('registersuccess.userdata-username') }} {{ $user->username }}</li>
		<li>{{ Lang::get('registersuccess.userdata-name') }} {{ $user->first_name }}</li>
		<li>{{ Lang::get('registersuccess.userdata-surname') }} {{ $user->surname }}</li>
		<li>{{ Lang::get('registersuccess.userdata-email') }} {{ $user->email }}</li>
	</ul>
	
	<p>{{ Lang::get('registersuccess.nextstep-description') }}</p>
	
	<div id="actionButtons">
		{{ HTML::link('/', Lang::get('registersuccess.nextstep-goback'), array('class'=>'btn btn-primary btn-lg')) }}
		{{ HTML::link('user/voiceaccess', Lang::get('registersuccess.nextstep-govoice'), array('class'=>'btn btn-primary btn-lg')) }}
	</div>

@stop