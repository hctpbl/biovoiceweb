@extends('master')

@section('content_center')
	
	<div class="page-header">
		<h2>{{ Lang::get('user.new') }}</h2>
	</div>
	
	@include('user.form')

@stop
