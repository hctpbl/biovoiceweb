<!DOCTYPE html>
<html lang="en">
  <head>

    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>
		@section('title')
			BioVoiceWeb
		@show
	</title>

    <!-- Bootstrap -->
    {{ HTML::style('css/bootstrap.min.css') }}
    {{ HTML::style('css/bvw.css') }}
    
    <!-- HTML5 Shim and Respond.js IE8 support of HTML5 elements and media queries -->
    <!-- WARNING: Respond.js doesn't work if you view the page via file:// -->
    <!--[if lt IE 9]>
      <script src="https://oss.maxcdn.com/libs/html5shiv/3.7.0/html5shiv.js"></script>
      <script src="https://oss.maxcdn.com/libs/respond.js/1.4.2/respond.min.js"></script>
    <![endif]-->

    @section('styles')
    @show
  </head>
  <body>

    <div class="navbar navbar-inverse navbar-fixed-top" role="navigation">
      <div class="container">
        <div class="navbar-header">
          <button type="button" class="navbar-toggle" data-toggle="collapse" data-target=".navbar-collapse">
            <span class="sr-only">{{ Lang::get('master.navbar-togglenav') }}</span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
          </button>
          <a class="navbar-brand" href="{{ URL::to('/') }}">BioVoiceWeb</a>
        </div>
        <div class="collapse navbar-collapse">
          <ul class="nav navbar-nav">
            <li class="{{ Request::is('/') ? 'active' : '' }}"><a href="{{ URL::to('/') }}">{{ Lang::get('master.navbar-home') }}</a></li>
            <li class="{{ Request::is('user/voiceaccess*') ? 'active' : '' }}"><a href="{{ URL::to('user/voiceaccess') }}">{{ Lang::get('master.navbar-voiceaccess') }}</a></li>
            <li><a href="#results">{{ Lang::get('master.navbar-results') }}</a></li>
          </ul>
        </div><!--/.nav-collapse -->
      </div>
    </div>
  
    <!-- Container -->
    <div class="container">

    	<div class="row">
	    	<div class="col-md-8">
				<div class="page-header" id="web-header">
					<h1>BioVoiceWeb</h1>
				</div>
		        <!-- Content_center -->
		        @yield('content_center')
	        </div>
	    	<div class="col-md-4">
		        <!-- Content_right -->
		        @yield('content_right')
	        </div>
        </div>

    </div>
    
    <div id="footer">
    	<div class="container">
    		<p>{{ Lang::get('master.footer') }}</p>
    	</div>
    </div>

    <!-- jQuery (necessary for Bootstrap's JavaScript plugins) -->
    {{ HTML::script('https://ajax.googleapis.com/ajax/libs/jquery/1.11.0/jquery.min.js') }}
    <!-- Include all compiled plugins (below), or include individual files as needed -->
    {{ HTML::script('js/bootstrap.min.js') }}

    @section('scripts')
    @show
    
  </body>
</html>