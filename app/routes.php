<?php

/*
|--------------------------------------------------------------------------
| Application Routes
|--------------------------------------------------------------------------
|
| Here is where you can register all of the routes for an application.
| It's a breeze. Simply tell Laravel the URIs it should respond to
| and give it the Closure to execute when that URI is requested.
|
*/

Route::controller('user', 'bvw\Controller\User');
Route::controller('api', 'bvw\Controller\Api');

Route::get('/', function()
{
	//return View::make('hello');
	return View::make('index');
});