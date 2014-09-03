	{{ Form::open(array('action' => 'bvw\Controller\User@postCreate')) }}
	
	<div class="form-group @if($errors->has('username'))has-error@endif">
		{{ Form::label('username', Lang::get('user.id')) }}
		{{ $errors->first('username', '<div class="alert alert-danger">:message</div>') }}
		{{ Form::text('username', Input::old('username'), array('class'=>'form-control', 'required'=>'')) }}
		<span class="help-block">{{ Lang::get('user.id-help') }}</span>
	</div>
	
	<div class="form-group @if($errors->has('first_name'))has-error@endif">
		{{ Form::label('first_name', Lang::get('user.first_name')) }}
		{{ $errors->first('first_name', '<div class="alert alert-danger">:message</div>') }}
		{{ Form::text('first_name', Input::old('first_name'), array('class'=>'form-control', 'required'=>'')) }}
	</div>
	
	<div class="form-group @if($errors->has('surname'))has-error@endif">
		{{ Form::label('surname', Lang::get('user.surname')) }}
		{{ $errors->first('surname', '<div class="alert alert-danger">:message</div>') }}
		{{ Form::text('surname', Input::old('surname'), array('class'=>'form-control')) }}
	</div>
	
	<div class="form-group @if($errors->has('email'))has-error@endif">
		{{ Form::label('email', Lang::get('user.email')) }}
		{{ $errors->first('email', '<div class="alert alert-danger">:message</div>') }}
		{{ Form::email('email', Input::old('email'), array('class'=>'form-control', 'required'=>'')) }}
	</div>
	
	<button type="submit" class="btn btn-default">{{ Lang::get('user.register') }}</button>
	
	{{ Form::close() }}