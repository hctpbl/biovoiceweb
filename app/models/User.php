<?php

namespace bvw\Model;

use \Eloquent;

use Illuminate\Auth\UserInterface;
use Illuminate\Auth\Reminders\RemindableInterface;

/**
 * Model for a User of the Speaker verification system
 * 
 * @author Héctor Pablos
 *
 */
class User extends Eloquent {

	/**
	 * The database table used by the model.
	 *
	 * @var string
	 */
	protected $table = 'user';
	
	/**
	 * For BVW, we use our own timestamp, not the ones
	 * provided by the eloquent class
	 * 
	 * @var boolean
	 */
	public $timestamps = false;

}