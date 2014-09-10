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
use \Session;
use GuzzleHttp\Client;
use GuzzleHttp\Exception\ClientException;

use \bvw\Model\User as ModelUser;

/**
 * User controller, whose function is to redirect petitios
 * related to users
 * 
 * @author Héctor Pablos
 *
 */
class User extends BaseController {
	
	/**
	 * Checks config file to return the URL for the API
	 * 
	 * @return string
	 */
	private function getAPIUrl() {
		return Config::get('bvapi.url');
	}
	
	/**
	 * Returns an array with the validation rules for the username
	 * @return array:string
	 */
	private function getUsernameRules() {
		return array (
			'username' => 'required|max:6|min:2|exists:user,username'
		);
	}

	public function getCreate() {
		return View::make('user/new')->withErrors(Session::get('errors'));
	}
	
	/**
	 * Returns the view to create a user
	 */
	public function postCreate() {
		$data = Input::all();
		
		$rules = array (
			'first_name' => 'required|max:30',
			'surname' => 'max:100',
			'username' => 'required|max:6|min:2',
			'email' => 'required|email|max:50',
		);
		
		$validator = Validator::make($data, $rules);
		
		if ($validator->passes()) {
			$client = new Client();
			try {
				$response = $jsonUser = $client->post($this->getAPIUrl()."/v1/users", [ 'body' =>
					[
						'username'=>$data['username'],
						'first_name'=>$data['first_name'],
						'surname'=>$data['surname'],
						'email'=>$data['email'],
					]
				]);
				$jsonUser = json_decode(json_encode($response->json()['user']));
				return View::make('user/registersuccess')->withUser($jsonUser);
			} catch (ClientException $exception) {
				$responseBody = $exception->getResponse();
				$jsonUser = json_decode(json_encode($responseBody->json()));
				return Redirect::to('/user/create')->withInput()->with('errors',$jsonUser->messages);
			}
		}
		
		return Redirect::to('/user/create')->withErrors($validator)->withInput();
	}
	
	/**
	 * Returns the viewto perform the voice acces for user $username
	 * @param string $username
	 * @return string
	 */
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
	
}
