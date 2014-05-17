<?php

namespace bvw\Controller;

use \BaseController;
use \Input;
use \Response;

class Api extends BaseController {
	
	public function postUploadAudio() {
		$file = Input::file('audio-blob');
		
		$destinationPath = 'uploads';
		$filename = str_random(12);
		//$filename = $file->getClientOriginalName();
		//$extension =$file->getClientOriginalExtension();
		$upload_success = $file->move($destinationPath, $filename.'.wav');
		
		if( $upload_success ) {
			return Response::json(array('status'=>'success', 'code'=>200));
		} else {
			return Response::json(array('status'=>'error', 'code'=>400));
		}
	}

}