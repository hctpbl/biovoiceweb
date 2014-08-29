<?php

namespace bvw\Controller;

use \BaseController;
use \Input;
use \Redirect;
use \Validator;
use \View;
use \Lang;
use \Log;
use \Config;
use GuzzleHttp\Client;

use \bvw\Model\User as ModelUser;

class User extends BaseController {
	
	private function getAPIUrl() {
		return Config::get('bvapi.url');
	}
	
	private function getUsernameRules() {
		return array (
			'username' => 'required|max:6|min:2|exists:user,username'
		);
	}
	
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
		$rules = $this->getUsernameRules();
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
			$client = new Client();
			$response = $client->get($this->getAPIUrl()."/v1/voiceaccess/status/".$username);
			$userStatus = $response->json();
			return View::make('user/speakerverification')
						->withUsername($username)
						->with('apiurl',$this->getAPIUrl())
						->with('registered',$userStatus["registered"])
						->with('enrolled',$userStatus["enrolled"]);
		}
		return "error usuario";
	}
	
	/*public function getTestvoiceaccess($username = null) {
		$rules = $this->getUsernameRules();
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
			return View::make('user/testspeaker')->withUsername($username)->with('apiurl',$this->getAPIUrl());
		}
		return "error usuario";
	}*/
	
}