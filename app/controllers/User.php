<?php

namespace bvw\Controller;

use \BaseController;
use \Input;
use \Redirect;
use \Validator;
use \View;
use \Lang;

use \bvw\Model\User as ModelUser;

class User extends BaseController {
	
	public function getCreate() {
		Return View::make('user/new');
	}
	
	public function postCreate() {
		$data = Input::all();
		
		$rules = array (
			'first_name' => 'required|max:30',
			'surname' => 'max:100',
			'username' => 'required|unique:user,username|max:6|min:2',
			'email' => 'required|email|unique:user,email|max:50',
		);
		
		$validator = Validator::make($data, $rules);
		
		if ($validator->passes()) {
			$usuario = new ModelUser;
			$usuario->first_name = $data['first_name'];
			$usuario->surname = $data['surname'];
			$usuario->username = $data['username'];
			$usuario->email = $data['email'];
			$usuario->save();
			return Redirect::to('/');
		}
		
		return Redirect::to('/user/create')->withErrors($validator)->withInput();
	}
	
	public function getVoiceaccess($username = null) {
		$rules = array (
			'username' => 'required|max:6|min:2|exists:user,username'
		);
		$messages = array (
			'username.exists' => Lang::get('user.verifyexistinguser')
		);
		
		if (Input::has('username')) {
			$username = Input::get('username');
		
			$validator = Validator::make(array('username'=>$username), $rules, $messages);
		
			if ($validator->passes()) {
				return Redirect::to('user/voiceaccess/'.$username);
			}
			
			return Redirect::to('user/voiceaccess')->withErrors($validator)->withInput();
		}
		if ($username == null) {
			Return View::make('user/voiceaccess');
		}
		$validator = Validator::make(array('username'=>$username), $rules);
		if ($validator->passes()) {
			return View::make('user/speakerverification')->withUsername($username);
		}
		return "error usuario";
	}
	
}