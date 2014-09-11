<?php
return array(

		/*
		|--------------------------------------------------------------------------
		| Voiceaccess Language Lines
		|--------------------------------------------------------------------------
		|
		| Lines for User related atributes in forms and views
		|
		*/
		
		'page-title' => 'Voice Access Page',
		'page-intro' => 'You are in the BioVoiceWeb voice access section, where you can test the substitution of access based on password by access based on voice.',
		'access-title' => 'Access',
		'access-instructions' => 'If it is your first time, please read the "Description" part of this page.',
		'access-warning' => '<strong>IMPORTANT: click on the correct button in accordance with the speech sample to be performed: authentic or impostor. This is important for a correct error rate calculation.</strong>',
		'description-title' => 'Description',
		'description-intro' => 'Steps to be followed:',
		'description-step1' => '<strong>Enrollment.</strong> Before access using your voice, you must enroll in the system, that is, you must provide speech samples for building your model.' .
		                       '<br><br>' .
							   'The duration of the speech samples provided should be of at least one minute, so the system can properly generate a complete model of them' . 
							   '<br><br>' .
							   'The system automatically detects if you are not enrolled and requests you to do so.',
		'description-step2' => '<b>Operation.</b> Once enrolled, you can test the recognition system. Before each voice access you will be informed if the speech sample is accepted or not. The classification threshold and the system output will also be shown.'
);
