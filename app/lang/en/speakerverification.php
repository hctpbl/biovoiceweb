<?php
return array(

		/*
		|--------------------------------------------------------------------------
		| Spekaer verification Lines
		|--------------------------------------------------------------------------
		|
		| Lines for Speaker verification related atributes in forms and views
		|
		*/

		'title' => 'Speaker verification for :username',
		'allowaudio' => 'To be able to record audio, you must first allow this page access to the microphone in your browser.',
		'not_enrolled_text'=>'This user is not enrolled yet, tap on the record button to start recording your voice and tap again to stop. Once you have recorded your voice enrollment, verification will be available. It is highly recommended that you record at least one minute of your voice for the system to generate an accurate model of it.',
    	'enrolled_text'=>'This user is already enrolled. You can re-enroll its voice or perform the user verification of your voice against him. Tap on the record button to start recording your voice and tap again to stop. Once you have recorded your voice, the different actions will be available.',
		'stop_recording_button'=>'Stop Recording',
		'start_recording_button'=>'Start Recording',
		'enroll_button'=>'Enroll',
		'verify_button'=>'Verify',
		'enrollment_modal_title'=>'Enrollment Successful',
		'enrollment_modal_text'=>'The user has been successfully enrolled. You can now re-enroll or perform the speaker verification against the model just created.',
		'modal_close'=>'Close',
		'verification_modal_title'=>'Verification results',
		'verification_modal_subtitle_success'=>'Possitive verification',
		'verification_modal_subtitle_failure'=>'Negative verification',
		'verification_modal_text_success'=>'The value obtained in the speaker verification process is higher than the declared threshold, so it is likely that the voice sample belongs to the user.',
		'verification_modal_text_failure'=>'The value obtained in the speaker verification process is lower than the declared threshold, so it is not likely that the voice sample belongs to the user.',
		'verification_result'=>'Result',
		'verification_threshold'=>'Threshold',
		
);