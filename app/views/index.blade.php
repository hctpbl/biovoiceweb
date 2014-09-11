@extends('master')

@section('content_center')

	<p>{{ Lang::get('index.bvw-description') }}</p>
	<p>{{ Lang::get('index.devices') }}</p>
	<ul>
		<li>{{ Lang::get('index.device-browser') }}</li>
		<li>{{ Lang::get('index.device-android') }}</li>
	</ul>
	
	<div class="page-header">
		<h2>{{ Lang::get('index.content-title') }}</h2>
	</div>
	<p>{{ Lang::get('index.content-intro') }}</p>
	<ul>
		<li>{{ Lang::get('index.content-home') }}</li>
		<li>{{ Lang::get('index.content-voice-access') }}</li>
		<!-- <li>{{ Lang::get('index.content-results') }}</li> -->
	</ul>
	
	<div class="page-header">
		<h2>{{ Lang::get('index.systemreg-title') }}</h2>
	</div>
	<p>{{ Lang::get('index.systemreg-intro') }}</p>
	<p>{{ Lang::get('index.systemreg-important') }}</p>
	<ul>
		<li>{{ Lang::get('index.systemreg-note1') }}</li>
		<li>{{ Lang::get('index.systemreg-note2') }}</li>
		<li>{{ Lang::get('index.systemreg-note3') }}</li>
		<li>{{ Lang::get('index.systemreg-note4') }}</li>
	</ul>
	
	<div class="page-header">
		<h2>{{ Lang::get('index.necsofthard-title') }}</h2>
	</div>
	<p>{{ Lang::get('index.necsofthard-intro') }}</p>
	<p>{{ Lang::get('index.necsofthard-pcaccess') }}</p>
	<ul>
		<li>{{ Lang::get('index.necsofthard-pcaccess-req1') }}</li>
		<li>{{ Lang::get('index.necsofthard-pcaccess-req2') }}</li>
	</ul>
	<p>{{ Lang::get('index.necsofthard-mobaccess') }}</p>
	<ul>
		<li>{{ Lang::get('index.necsofthard-mobaccess-req1') }}</li>
		<li>{{ Lang::get('index.necsofthard-mobaccess-req2') }}</li>
	</ul>
	
@stop

@section('content_right')
	<div class="date">
		<p>{{ Carbon\Carbon::now()->toDayDateTimeString() }}</p>
	</div>
	
	<div class="panel panel-default">
		<div class="panel-heading">
			<h3 class="panel-title">{{ Lang::get('index.newspanel-title') }}</h3>
		</div>
		<div class="panel-body">
			<p>{{ Lang::get('index.newspanel-registeredusers', array('regusersnum' => '0')) }}</p>
		</div>
	</div>
	
	<div class="panel panel-default">
		<div class="panel-heading">
			<h3 class="panel-title">{{ Lang::get('index.downloadspanel-title') }}</h3>
		</div>
		<div class="panel-body">
			<p>{{ Lang::get('index.downloadspanel-app1') }}</p>
			<ul>
				<li>{{ Lang::get('index.downloadspanel-app1reqs-title') }}</li>
				<li><ul>
					<li>{{ Lang::get('index.downloadspanel-app1reqs-req1') }}</li>
					<li>{{ Lang::get('index.downloadspanel-app1reqs-req2') }}</li>
				</ul></li>
			</ul>
			<ul>
				<li>{{ Lang::get('index.downloadspanel-app1inst-title') }}</li>
				<li><ol type="a">
					<li>{{ Lang::get('index.downloadspanel-app1inst-step1') }}</li>
					<li>{{ Lang::get('index.downloadspanel-app1inst-step2') }}</li>
					<li>{{ Lang::get('index.downloadspanel-app1inst-step3') }}</li>
				</ol></li>
			</ul>
		</div>
	</div>
	
	<div class="panel panel-default">
		<div class="panel-heading">
			<h3 class="panel-title">{{ Lang::get('index.registerpanel-title') }}</h3>
		</div>
		<div class="panel-body">
			@include('user.form')
		</div>
	</div>
@stop
