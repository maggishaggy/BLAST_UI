<?php

/**
 * @file
 * Contains all functions for the Protein BLAST
 */


/**
 * Protein BLAST Submission Form
 *
 * @see blast_protein_form_validate()
 * @see blast_protein_form_submit()
 */
function blast_protein_form($form, &$form_state) {

	//  CSS support to the form
	$form['#attached']['css'] = array(
		drupal_get_path('module', 'blast_ui') . '/css/form.css',
		);

  // PROTEIN QUERY
  //.........................

  $form['query'] = array(
    '#type' => 'fieldset',
    '#title' => t('Enter Query Sequence'),
    '#description' => t('Enter one or more queries in the top text box or use the browse button to upload a file from your local disk. The file may contain a single sequence or a list of sequences. In both cases, the data must be in FASTA format. <a href="http://www.ncbi.nlm.nih.gov/BLAST/blastcgihelp.shtml" target="_blank">More information..</a> '),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
   	'#prefix' => '<div class="two-col">',
    '#suffix' => '</div>', 
  );

  $form['query']['FASTA'] = array(
    '#type' => 'textarea',
    '#title' => t('Enter FASTA sequence(s)'),
    '#description'=>t('Enter query sequence(s) in the text area.'),
    '#prefix' => '<div id="fasta_seq">',
    '#suffix' => '</div>',
  );

  // Upload a file as an alternative to enter a query sequence
  $form['#attributes']['enctype'] = 'multipart/form-data';
  $form['query']['UPLOAD'] = array(
  	'#type' => 'file',
  	'#prefix' => '<div class="col1"> Or upload your query files:',
    '#suffix' => '</div>',
  	'#description' => t('The file should be a plain-text FASTA file and not a .doc, .docx, etc. It cannot be greater than 10 Mb in size.'),
  );
 
    
	$form['query']['example_sequence'] = array(
			'#type' => 'button', 
			'#button_type'=> 'button',
			'#limit_validation_errors' => array(),
			'#value' => t('Example Sequence'), 
			'#prefix' => '<div class="col1">',
			'#suffix' => '</div>',
			'#validate' => array(),
			'#ajax' => array(
				'callback' => 'ajax_protein_text_area_callback',
				'wrapper'  => 'fasta_seq',
					'method'   => 'replace',
					'effect'   => 'fade',
					),
		'#attributes' => array('onclick' => 'return false;'),
		);


  // BLAST DATABASE
  //.........................

  $form['DB'] = array(
    '#type' => 'fieldset',
    '#title' => t('Choose Search Set'),
    '#description' => t('Choose from one of the protein BLAST databases listed below. You can also use the browse button to upload a file from your local disk. The file may contain a single sequence or a list of sequences. '),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $options = get_blast_database_options('p');
  $form['DB']['SELECT_DB'] = array(
  '#type' => 'select',
   '#title' => t('Protein BLAST Databases:'),
   '#options' => $options,
   '#default_value' => t('Select a database'),
  );

  // Upload a file as an alternative to enter a query sequence
  $form['#attributes']['enctype'] = 'multipart/form-data';
  $form['DB']['DBUPLOAD'] = array(
   '#prefix' => 'Or upload your own dataset:	',
   '#type' => 'file',
   '#description' => t('The file should be a plain-text FASTA file and not a .doc, .docx, etc. It cannot be greater than 10 Mb in size.'),
  );

  // ALGORITHM PARAMETERS
  //.........................

	$form['ALG'] = array(
    '#type' => 'fieldset',
    '#title' => t('Algorithm parameters'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  //General parameters

  $form['ALG']['GParam'] = array(
    '#type' => 'fieldset',
    '#title' => t('General parameters'),
    '#collapsible' => FALSE,
  );

  $form['ALG']['GParam']['maxTarget'] = array(
    '#type' => 'select',
    '#title' => t('Max target sequences:'),
    '#options' => array(
       0 => t('10'),
       1 => t('50'),
       2 => t('100'),
       3 => t('250'),
       4 => t('500'),
       5 => t('1000'),
       6 => t('5000'),
       7 => t('10000'),
       8 => t('20000'),
    ),
    '#default_value' => 2,
    '#description' => t('Select the maximum number of aligned sequences to display'),
  );

  $form['ALG']['GParam']['shortQueries'] = array(
   '#type' => 'checkbox',
   '#title' => t('Automatically adjust parameters for short input sequences'),
   '#default_value' => TRUE,
  );

  $form['ALG']['GParam']['eVal'] = array(
  	'#type' => 'textfield',
  	'#title' => t('e-value(Expect threshold)'),
  	'#default_value' => 10,
  	'#size' => 12,
  	'#maxlength' => 20,
  	'#description' => t('Expected number of chance matches in a random model.'),
  );

  $form['ALG']['GParam']['wordSize'] = array(
    '#type' => 'select',
    '#title' => t('Word size:'),
    '#options' => array(
       0 => t('2'),
       1 => t('3'),
    ),
    '#default_value' => 1,
    '#description' => t('The length of the seed that initiates an alignment'),
  );

  $form['ALG']['GParam']['qRange'] = array(
   '#type' => 'textfield',
   '#title' => t('Max matches in a query range'),
   '#default_value' => 0,
   '#size' => 12,
   '#maxlength' => 20,
   '#description' => t('Limit the number of matches to a query range. This option is useful if many strong matches to one part of a query may prevent BLAST from presenting weaker matches to another part of the query.'),
  );

  // Scoring parameters

  $form['ALG']['SParam'] = array(
   '#type' => 'fieldset',
   '#title' => t('Scoring parameters'),
   '#collapsible' => FALSE,
  );

  $options_first = _ajax_example_get_first_dropdown_options();
  $selected = isset($form_state['values']['MATRIX'] ) ? $form_state['values']['MATRIX'] : key($options_first);

  $form['ALG']['SParam']['MATRIX'] = array(
    '#type' => 'select',
    '#title' => 'Matrix',
    '#options' => $options_first,
    '#default_value' => $selected,
    '#description' => t('Assigns a score for aligning pairs of residues, and determines overall alignment score..'),
    '#ajax' => array(
      'callback' => 'ajax_example_dependent_dropdown_callback',
      'wrapper' => 'dropdown-second-replace',
    ),
  );

  $form['ALG']['SParam']['gapCost'] = array(
    '#type' => 'select',
    '#title' => t('Gap Costs:'),
    '#prefix' => '<div id="dropdown-second-replace">',
    '#suffix' => '</div>',
    '#options' => _ajax_example_get_second_dropdown_options($selected),
    '#default_value' => 2,
    '#description' => t('Cost to create and extend a gap in an alignment.'),
  );

  $form['ALG']['SParam']['M&MScores'] = array(
    '#type' => 'select',
    '#title' => t('Match/Mismatch Scores:'),
    '#options' => array(
      0 => t('No adjustment'),
      1 => t('Composition-based statistics'),
      2 => t('Conditional compositional score matrix adjustment'),
      3 => t('Universal composition score matrix adjustment '),
    ),
    '#default_value' => 2,
    '#description' => t('Matrix adjustment method to compensate for amino acid composition of sequences'),
  );

  //Submit
  $form['submit'] = array(
    '#type' => 'submit',
    '#default_value' => ' BLAST ',
  );
 $form['reset'] = array(
    '#type' => 'markup',
    '#markup' => '<input type="reset" value="Reset" class="form-submit">',
	);

  return $form;
}

/**
 * Form validation handler for blast_protein_form().
 *
 * @see blast_protein_form_validate()
 */
function blast_protein_form_validate($form, &$form_state) {

	// Get the sequence 
  $fastaSeq = $form_state['values']['FASTA'];
  $upQuery = file_save_upload('UPLOAD', array('file_validate_extensions' => array('txt fasta fa fna'),
  																						'file_validate_size' => array(2*1024*1024),
  																						), FILE_EXISTS_RENAME);
  if(empty($fastaSeq) && empty($upQuery)) {
  	form_set_error('FASTA', t('Enter the FASTA sequence below or you have to upload the FASTA file with the sequence '));
  }
   
  // Get the DB
  $db_selected = $form_state['values']['SELECT_DB']; 
  $upDB = file_save_upload('DBUPLOAD', array('file_validate_extensions' => array('txt fasta fa fna'),
  																					 'file_validate_size' => array(2*1024*1024),
  																						), FILE_EXISTS_RENAME);
  if(empty($upDB) && $db_selected == 0) {
  	form_set_error('SELECT_DB', t('Select the database from the list or upload the FASTA file'));
  }
  

  // Validate query sequence
  if (isset($fastaSeq)) {
    if (validateFasta($fastaSeq)){
      form_set_error('pBLAST', t('Error: Failed to read the Blast query: Wrong format provided for FASTA protein sequence'));
    }
    else {
      $form_state['qFlag'] = 'seqQuery';
    }
  }

  // Validate Query Upload
  if ($upQuery) {
    $upQuery_uri = $upQuery->uri;
    $form_state['upQuery_path'] = drupal_realpath($upQuery_uri);
    $upQuery_content = file_get_contents($form_state['upQuery_path']);
    if(validateFasta($upQuery_content)){
      form_set_error('pBLAST', t('Error: Failed to upload the Blast query: Wrong format provided for FASTA protein sequence'));
    }
    else {
      $form_state['qFlag'] = 'upQuery';
    }
  }

  // Validate uploaded database
  if ($upDB) {
    $upDB_uri = $upDB->uri;
    $form_state['upDB_path'] = drupal_realpath($upDB_uri);
    $upDB_content = file_get_contents($form_state['upDB_path']);
    if(validateFasta($upDB_content)){
      form_set_error('DB', t('Error: Failed to upload the Blast subject sequence file: Wrong format provided for FASTA protein sequence'));
    }
    else {
      $form_state['dbFlag'] = 'upQuery';
    }
  }
  else {
    $form_state['dbFlag'] = 'blastdb';
  }

}

/**
 * Form submition handler for blast_protein_form().
 *
 * @see blast_protein_form_submit()
 */
function blast_protein_form_submit($form, &$form_state) {

  $eVal = $form_state['values']['eVal'];

  $trgtKey = $form_state['values']['maxTarget'];
  $numAlign = $form['ALG']['GParam']['maxTarget']['#options'][$trgtKey];

  $wsKey = $form_state['values']['wordSize'];
  $wordSize = $form['ALG']['GParam']['wordSize']['#options'][$wsKey];

  // Expand Gap Cost key into open and extend penalties
  $gapKey = $form_state['values']['MATRIX'];
  switch ($gapKey) {
   case 0:
     $matrix ="PAM30";
     $gapKey = $form_state['values']['gapCost'];
     switch ($gapKey) {
      case 0:
         $gapOpen = 7;
         $gapExtend = 2;
         break;
      case 1:
         $gapOpen = 6;
         $gapExtend = 2;
         break;
      case 2:
         $gapOpen = 5;
         $gapExtend = 2;
         break;
      case 3:
         $gapOpen = 10;
         $gapExtend = 1;
         break;
      case 4:
         $gapOpen = 9;
         $gapExtend = 1;
         break;
      case 5:
         $gapOpen = 8;
         $gapExtend = 1;
         break;
     }
     break;
   case 1:
     $matrix ="PAM70";
     $gapKey = $form_state['values']['gapCost'];
     switch ($gapKey) {
      case 0:
         $gapOpen = 8;
         $gapExtend = 2;
         break;
      case 1:
         $gapOpen = 7;
         $gapExtend = 2;
         break;
      case 2:
         $gapOpen = 6;
         $gapExtend = 2;
         break;
      case 3:
         $gapOpen = 11;
         $gapExtend = 1;
         break;
      case 4:
         $gapOpen = 10;
         $gapExtend = 1;
         break;
      case 5:
         $gapOpen = 9;
         $gapExtend = 1;
         break;
     }
     break;
   case 2:
     $matrix ="PAM250";
     $gapKey = $form_state['values']['gapCost'];
     switch ($gapKey) {
      case 0:
         $gapOpen = 15;
         $gapExtend = 3;
         break;
      case 1:
         $gapOpen = 14;
         $gapExtend = 3;
         break;
      case 2:
         $gapOpen = 13;
         $gapExtend = 3;
         break;
      case 3:
         $gapOpen = 12;
         $gapExtend = 3;
         break;
      case 4:
         $gapOpen = 11;
         $gapExtend = 3;
         break;
      case 5:
         $gapOpen = 17;
         $gapExtend = 2;
         break;
      case 6:
         $gapOpen = 16;
         $gapExtend = 2;
         break;
      case 7:
         $gapOpen = 15;
         $gapExtend = 2;
         break;
      case 8:
         $gapOpen = 14;
         $gapExtend = 2;
         break;
      case 9:
         $gapOpen = 13;
         $gapExtend = 2;
         break;
      case 10:
         $gapOpen = 21;
         $gapExtend = 1;
         break;
      case 11:
         $gapOpen = 20;
         $gapExtend = 1;
         break;
      case 12:
         $gapOpen = 19;
         $gapExtend = 1;
         break;
      case 13:
         $gapOpen = 18;
         $gapExtend = 1;
         break;
      case 14:
         $gapOpen = 17;
         $gapExtend = 1;
         break;
     }
     break;
   case 3:
     $matrix ="BLOSUM80";
     $gapKey = $form_state['values']['gapCost'];
     switch ($gapKey) {
      case 0:
         $gapOpen = 8;
         $gapExtend = 2;
         break;
      case 1:
         $gapOpen = 7;
         $gapExtend = 2;
         break;
      case 2:
         $gapOpen = 6;
         $gapExtend = 2;
         break;
      case 3:
         $gapOpen = 11;
         $gapExtend = 1;
         break;
      case 4:
         $gapOpen = 10;
         $gapExtend = 1;
         break;
      case 5:
         $gapOpen = 9;
         $gapExtend = 1;
         break;
     }
      break;
   case 4:
     $matrix ="BLOSUM62";
     $gapKey = $form_state['values']['gapCost'];
     switch ($gapKey) {
      case 0:
         $gapOpen = 11;
         $gapExtend = 2;
         break;
      case 1:
         $gapOpen = 10;
         $gapExtend = 2;
         break;
      case 2:
         $gapOpen = 9;
         $gapExtend = 2;
         break;
      case 3:
         $gapOpen = 8;
         $gapExtend = 2;
         break;
      case 4:
         $gapOpen = 7;
         $gapExtend = 2;
         break;
      case 5:
         $gapOpen = 6;
         $gapExtend = 2;
         break;
      case 6:
         $gapOpen = 13;
         $gapExtend = 1;
         break;
      case 7:
         $gapOpen = 12;
         $gapExtend = 1;
         break;
      case 8:
         $gapOpen = 11;
         $gapExtend = 1;
         break;
      case 9:
         $gapOpen = 10;
         $gapExtend = 1;
         break;
      case 10:
         $gapOpen = 9;
         $gapExtend = 1;
         break;
     }
      break;
   case 5:
     $matrix ="BLOSUM45";
     $gapKey = $form_state['values']['gapCost'];
     switch ($gapKey) {
      case 0:
         $gapOpen = 13;
         $gapExtend = 3;
         break;
      case 1:
         $gapOpen = 12;
         $gapExtend = 3;
         break;
      case 2:
         $gapOpen = 11;
         $gapExtend = 3;
         break;
      case 3:
         $gapOpen = 10;
         $gapExtend = 3;
         break;
      case 4:
         $gapOpen = 15;
         $gapExtend = 2;
         break;
      case 5:
         $gapOpen = 14;
         $gapExtend = 2;
         break;
      case 6:
         $gapOpen = 13;
         $gapExtend = 2;
         break;
      case 7:
         $gapOpen = 12;
         $gapExtend = 2;
         break;
      case 8:
         $gapOpen = 19;
         $gapExtend = 1;
         break;
      case 9:
         $gapOpen = 18;
         $gapExtend = 1;
         break;
      case 10:
         $gapOpen = 17;
         $gapExtend = 1;
         break;
      case 11:
         $gapOpen = 16;
         $gapExtend = 1;
         break;
     }
     break;
   case 6:
     $matrix ="BLOSUM50";
     $gapKey = $form_state['values']['gapCost'];
     switch ($gapKey) {
      case 0:
         $gapOpen = 13;
         $gapExtend = 3;
         break;
      case 1:
         $gapOpen = 12;
         $gapExtend = 3;
         break;
      case 2:
         $gapOpen = 11;
         $gapExtend = 3;
         break;
      case 3:
         $gapOpen = 10;
         $gapExtend = 3;
         break;
      case 4:
         $gapOpen = 9;
         $gapExtend = 3;
         break;
      case 5:
         $gapOpen = 16;
         $gapExtend = 2;
         break;
      case 6:
         $gapOpen = 15;
         $gapExtend = 2;
         break;
      case 7:
         $gapOpen = 14;
         $gapExtend = 2;
         break;
      case 8:
         $gapOpen = 13;
         $gapExtend = 2;
         break;
      case 9:
         $gapOpen = 12;
         $gapExtend = 2;
         break;
      case 10:
         $gapOpen = 19;
         $gapExtend = 1;
         break;
      case 11:
         $gapOpen = 18;
         $gapExtend = 1;
         break;
      case 12:
         $gapOpen = 17;
         $gapExtend = 1;
         break;
      case 13:
         $gapOpen = 16;
         $gapExtend = 1;
         break;
      case 14:
         $gapOpen = 15;
         $gapExtend = 1;
         break;
     }
     break;
   case 7:
     $matrix ="BLOSUM90";
     $gapKey = $form_state['values']['gapCost'];
     switch ($gapKey) {
      case 0:
         $gapOpen = 9;
         $gapExtend = 2;
         break;
      case 1:
         $gapOpen = 8;
         $gapExtend = 2;
         break;
      case 2:
         $gapOpen = 7;
         $gapExtend = 2;
         break;
      case 3:
         $gapOpen = 6;
         $gapExtend = 2;
         break;
      case 4:
         $gapOpen = 11;
         $gapExtend = 1;
         break;
      case 5:
         $gapOpen = 10;
         $gapExtend = 1;
         break;
      case 6:
         $gapOpen = 9;
         $gapExtend = 1;
         break;
     }
     break;
  }

  // If the query was submitted via the texrfield then create a file containing it
  if ( isset($form_state['qFlag']) ) {
    if ( $form_state['qFlag'] == 'seqQuery' ) {
      $seq_content = $form_state['values']['FASTA'];
	    $query = "/tmp/user__query_file.fasta";
      file_put_contents ( $query , $seq_content);
    }
    elseif ( $form_state['qFlag'] == 'upQuery' ) {
      $query = $form_state['upQuery_path'];
    }
  }

  // If the BLAST database was uploaded then use it to run the BLAST
  if ($form_state['dbFlag'] == 'upQuery') {

    // Since we only support using the -db flag (not -subject) we need to create a
    // blast database for the FASTA uploaded.
    // NOTE: We can't support subject because we need to generate the ASN.1+ format
    // to provide multiple download type options from the same BLAST
    $blastdb_with_path = $form_state['upDB_path'];
    system("makeblastdb -in $blastdb_with_path -dbtype prot -parse_seqids");

  }
  // Otherwise, we are using one of the website provided BLAST databases so form the
  // BLAST command accordingly
  elseif ($form_state['dbFlag'] == 'blastdb') {

     $selected_db = $form_state['values']['SELECT_DB'];
     $blastdb_node = node_load($selected_db);
     $blastdb_with_path = $blastdb_node->db_path;

  }

  // Actually submit the BLAST Tripal Job
  // NOTE: Tripal jobs needs to be executed from the command-line before it will be run!!
  $blastdb_with_suffix = $blastdb_with_path . '.psq';
  if (is_readable($blastdb_with_suffix)) {
    global $user;
    $output_filestub = date('YMd_His');
    $job_args = array(
      'program' => 'blastp',
      'query' => $query,
      'database' => $blastdb_with_path,
      'output_filename' => $output_filestub,
      'options' => array(
        'evalue' => $eVal,
        'word_size' => $wordSize,
        'gapopen' => $gapOpen,
        'gapextend' => $gapExtend,
        'matrix' => $matrix
      )
    );
    $job_id = tripal_add_job("BLAST (blastp): $query",'blast_job','run_BLAST_tripal_job', $job_args, $user->uid);
	//	tripal_jobs_launch($do_parallel = 0);
    // Redirect to the BLAST results page
    drupal_goto("blast/report/$job_id");
  }
  else {
    $dbfile_uploaded_msg = ($form_state['dbFlag'] == 'upQuery') ? 'The BLAST database was submitted via user upload.' : 'Existing BLAST Database was chosen';
    tripal_report_error(
      'blast_ui',
      TRIPAL_ERROR,
      "BLAST database %db unaccessible. $dbfile_uploaded_msg",
      array('%db' => $blastdb_with_path)
    );
    drupal_set_message('BLAST database unaccessible. Please contact the site administrator.','error');
  }
}


/**
 * FASTA validating parser
 *
 * @param $sequence
 *  A string of characters to be validated. A sequence in FASTA format begins with a single-line description, followed by lines of sequence data.
 *  The description line is distinguished from the sequence data by a greater-than (">") symbol in the first column.
 *  The word following the ">" symbol is the identifier of the sequence, and the rest of the line is the description (both are optional).
 *  There should be no space between the ">" and the first letter of the identifier. The sequence ends if another line starting with a ">" appears;
 *  this indicates the start of another sequence.
 *
 * @return
 *  Return a boolean. 1 if the sequence does not pass the format valifation stage and 0 otherwise.
 *
 */
function validateFasta($sequence) {
	$fastaIdRegEx = '/^>.*(\\n|\\r)/';
	$fastaSeqRegEx = '/[^acgturykmswbdhvnxACGTURYKMSWBDHVNX\*\-\n\r]/';
	if ( preg_match($fastaSeqRegEx,$sequence) && !(preg_match($fastaIdRegEx,$sequence)) ) {
    $flag = 1;
  } else {
    $flag = 0;
  }

  return $flag;
}

/**
 * Fill the first dropdown list with appropriate options
 *
 * @return
 * An array consisting of matrices name for the first dropdown list
 */
function _ajax_example_get_first_dropdown_options() {
  return drupal_map_assoc(array(
  t('PAM30'),
  t('PAM70'),
  t('PAM250'),
  t('BLOSUM80'),
  t('BLOSUM62'),
  t('BLOSUM45'),
  t('BLOSUM50'),
  t('BLOSUM90'),
  ));
}

/**
 * Fill the second dropdown list with appropriate options
 *
 * @return
 * An array containing open and extension gap values for the chosen matrix (to fill the second dropdown list)
 */
function _ajax_example_get_second_dropdown_options($key = '') {
  $options = array(
    t('PAM30') => drupal_map_assoc(array(
      t('Existence: 7 Extension: 2'),
      t('Existence: 6 Extension: 2'),
      t('Existence: 5 Extension: 2'),
      t('Existence: 10 Extension: 1'),
      t('Existence: 9 Extension: 1'),
      t('Existence: 8 Extension: 1'),
     )),
    t('PAM70') => drupal_map_assoc(array(
      t('Existence: 8 Extension: 2'),
      t('Existence: 7 Extension: 2'),
      t('Existence: 6 Extension: 2'),
      t('Existence: 11 Extension: 1'),
      t('Existence: 10 Extension: 1'),
      t('Existence: 9 Extension: 1'),
    )),
    t('PAM250') => drupal_map_assoc(array(
      t('Existence: 15 Extension: 3'),
      t('Existence: 14 Extension: 3'),
      t('Existence: 13 Extension: 3'),
      t('Existence: 12 Extension: 3'),
      t('Existence: 11 Extension: 3'),
      t('Existence: 17 Extension: 2'),
      t('Existence: 16 Extension: 2'),
      t('Existence: 15 Extension: 2'),
      t('Existence: 14 Extension: 2'),
      t('Existence: 13 Extension: 2'),
      t('Existence: 21 Extension: 1'),
      t('Existence: 20 Extension: 1'),
      t('Existence: 19 Extension: 1'),
      t('Existence: 18 Extension: 1'),
      t('Existence: 17 Extension: 1'),
    )),
    t('BLOSUM80') => drupal_map_assoc(array(
      t('Existence: 8 Extension: 2'),
      t('Existence: 7 Extension: 2'),
      t('Existence: 6 Extension: 2'),
      t('Existence: 11 Extension: 1'),
      t('Existence: 10 Extension: 1'),
      t('Existence: 9 Extension: 1'),
    )),
    t('BLOSUM62') => drupal_map_assoc(array(
      t('Existence: 11 Extension: 2'),
      t('Existence: 10 Extension: 2'),
      t('Existence: 9 Extension: 2'),
      t('Existence: 8 Extension: 2'),
      t('Existence: 7 Extension: 2'),
      t('Existence: 6 Extension: 2'),
      t('Existence: 13 Extension: 1'),
      t('Existence: 12 Extension: 1'),
      t('Existence: 11 Extension: 1'),
      t('Existence: 10 Extension: 1'),
      t('Existence: 9 Extension: 1'),
    )),
    t('BLOSUM45') => drupal_map_assoc(array(
      t('Existence: 13 Extension: 3'),
      t('Existence: 12 Extension: 3'),
      t('Existence: 11 Extension: 3'),
      t('Existence: 10 Extension: 3'),
      t('Existence: 15 Extension: 2'),
      t('Existence: 14 Extension: 2'),
      t('Existence: 13 Extension: 2'),
      t('Existence: 12 Extension: 2'),
      t('Existence: 19 Extension: 1'),
      t('Existence: 18 Extension: 1'),
      t('Existence: 17 Extension: 1'),
      t('Existence: 16 Extension: 1'),
    )),
    t('BLOSUM50') => drupal_map_assoc(array(
      t('Existence: 13 Extension: 3'),
      t('Existence: 12 Extension: 3'),
      t('Existence: 11 Extension: 3'),
      t('Existence: 10 Extension: 3'),
      t('Existence: 9 Extension: 3'),
      t('Existence: 16 Extension: 2'),
      t('Existence: 15 Extension: 2'),
      t('Existence: 14 Extension: 2'),
      t('Existence: 13 Extension: 2'),
      t('Existence: 12 Extension: 2'),
      t('Existence: 19 Extension: 1'),
      t('Existence: 18 Extension: 1'),
      t('Existence: 17 Extension: 1'),
      t('Existence: 16 Extension: 1'),
      t('Existence: 15 Extension: 1'),
    )),
    t('BLOSUM90') => drupal_map_assoc(array(
      t('Existence: 9 Extension: 2'),
      t('Existence: 8 Extension: 2'),
      t('Existence: 7 Extension: 2'),
      t('Existence: 6 Extension: 2'),
      t('Existence: 11 Extension: 1'),
      t('Existence: 10 Extension: 1'),
      t('Existence: 9 Extension: 1'),
    )),
    );
    if (isset($options[$key])) {
    	return $options[$key];
    } else {
    	return array();
    }
}

/**
 * Respond to Ajax dropdown call
 */
function ajax_example_dependent_dropdown_callback($form, $form_state) {
  return $form['ALG']['SParam']['gapCost'];
}


// call back function for example sequence
function ajax_protein_text_area_callback($form, $form_state) {
 $element = $form['query']['FASTA']; // Get example Protein sequence
 
$element['#value'] =  '>lipoxygenase Glyma15g03040  
TTTCGTATGA GATTAAAATG TGTGAAATTT TGTTTGATAG GACATGGGAA AGGAAAAGTT GGAAAGGCTA CAAATTTAAG AGGACAAGTG TCGTTACCAA CCTTGGGAGC TGGCGAAGAT GCATACGATG TTCATTTTGA ATGGGACAGT GACTTCGGAA TTCCCGGTGC ATTTTACATT AAGAACTTCA TGCAAGTTGA GTTCTATCTC AAGTCTCTAA CTCTCGAAGA CATTCCAAAC CACGGAACCA TTCACTTCGT ATGCAACTCC TGGGTTTACA ACTCAAAATC CTACCATTCT GATCGCATTT TCTTTGCCAA CAATGTAAGC TACTTAAATA CTGTTATACA TTGTCTAACA TCTTGTTAGA GTCTTGCATG ATGTGTACCG TTTATTGTTG TTGTTGAACT TTACCACATG GCATGGATGC AAAAGTTGTT ATACACATAA ATTATAATGC AGACATATCT TCCAAGCGAG ACACCGGCTC CACTTGTCAA GTACAGAGAA GAAGAATTGA AGAATGTAAG AGGGGATGGA ACTGGTGAGC GCAAGGAATG GGATAGGATC TATGATTATG ATGTCTACAA TGACTTGGGC GATCCAGATA AGGGTGAAAA GTATGCACGC CCCGTTCTTG GAGGTTCTGC CTTACCTTAC CCTCGCAGAG GAAGAACCGG AAGAGGAAAA ACTAGAAAAG GTTTCTCACT AGTCACTAAT TTATTACTTT TTAATGTTTG TTTTTAGGCA TCTTTTCTGA TGAAATGTAT ACTTTTGATG TTTTTTTGTT TTAGCATAAC TGAATTAGTA AAGTGTGTTG TGTTCCTTAG AAGTTAGAAA AGTACTAAGT ATAAGGTCTT TGAGTTGTCG TCTTTATCTT AACAGATCCC AACAGTGAGA AGCCCAGTGA TTTTGTTTAC CTTCCGAGAG ATGAAGCATT TGGTCACTTG AAGTCATCAG ATTTTCTCGT TTATGGAATC AAATCAGTGG CTCAAGACGT CTTGCCCGTG TTGACTGATG CGTTTGATGG CAATCTTTTG AGCCTTGAGT TTGATAACTT TGCTGAAGTG CGCAAACTCT ATGAAGGTGG AGTTACACTA CCTACAAACT TTCTTAGCAA GATCGCCCCT ATACCAGTGG TCAAGGAAAT TTTTCGAACT GATGGCGAAC AGTTCCTCAA GTATCCACCA CCTAAAGTGA TGCAGGGTAT GCTACATATT TTGAATATGT AGAATATTAT CAATATACTC CTGTTTTTAT TCAACATATT TAATCACATG GATGAATTTT TGAACTGTTA TTATTTGGTG AAGTGGATAA GTCTGCATGG ATGACTGATG AAGAATTTGC TAGAGAAACG ATTGCTGGTG TTAATCCAAA TGTCATTAAG ATTCTTGAGG TAACAAACTA CATGCAACCA AAAGCTTGGA TGCACTTTGG TTGTCAAATT TGAGATTGCT AACCTGATGT CATTGTGTGC TAAATATATT CCAGGAGTTC CCACCACGTA GCAAGCTAGA TTCTCAAGCC TACGGTGATC ATACCTCTAT AATAACAAAA CAACACTTGG AGCCTAACTT GGGTGGGCTC ACCGTTGAGC AGGTAATGAT ATTAGTATTT AGTTTCACAT TAAAGTTATT TCACTATAAT GTTTCTGCTA CTGCTTTGTA ACTATTTTTG TTTTTTTATT TTTTTATTGT GATTCCAATG AACAACAATC AACCAATTAA TTAATTAATT AATTATACTT GGTTGAACAG GCTATCCAGA GCAAGAAGTT GTTCATCTTA GATCACCATG ACTATCTCAT TCCATATTTG AGGAAAATAA ATGCAACTAC CACAAAGACT TACGCTACAA GAACCATATT TTTCTTGAAA AGTGATGGAA CCTTGACGCC ATTGGCCATT GAGTTAAGTA AGCCGCATCC TCAGGGTGAA GGATATGGTC CTGTTAGCGA AGTCTACGTG CCTTCGAGCG AGGGAGTTGA AGCTTACATT TGGTTACTGG CAAAGGCTTA TGTTGTTGTG AATGATTCGT GCTACCATCA ACTCGTCAGC CACTGGTATG ATATTGTCAA TTGGCAATGA GATGCATACA ATTAAATTTG CCAATATATT TATAAATAGA TGATTAACAT ATTAACGTGG ATGGCAGGCT AAACACTCAT GCGGTTGTTG AGCCATTCGT CATAGCAACA AACAGGCATC TGAGCGTGGT TCACCCTATT TACAAACTTT TGTTTCCTCA CTATCGTGAC ACCATGAACA TTAATTCACT TGCCCGCAAA TCCTTGGTCA ATGCAGACGG TATTATAGAG AAAACATTCT TGTGGGGTAG GTACTCTTTG GAAATGTCTG CTGTTATTTA CAAGGACTGG GTTTTCACTG ATCAAGCATT GCCTAATGAT CTTGTCAAGA GGTATGTAAT CTTGTAAAGT TTATTAACAA ATTAGAAATA TATATTGCTG AAAAAATAAA GAATCTAAAT CTATTTAAGT CATCAATGAC ATAAGAAGAA AAAAAGGCCT CAGGCTTGAT ATAACTCTTG TGCTAATCAA TGTTATTTGG GTGCTTTTAA TTCAGAGGAG TTGCGGTTAA GGATCCATCT GCTCCCCATG GAGTTCGGCT TTTGATTGAG GACTATCCTT ATGCTTCTGA TGGGCTAGAG ATATGGGATG CCATCAAGTC CTGGGTGCAT GAATATGTCT CATTCTACTA CAAGTCAGAT GCAGCAATTC AACAAGATCC CGAGCTCCAA GCTTGGTGGA AAGAACTTGT CCAAGTGGGT CATGGTGATT TGAAAGATAA GCCATGGTGG CAAAAGATGC AAACTCGTGA AGAGTTGATT GAAGCCTCGG CTACCCTCGT ATGGATCGCT TCAGCCCTTC ATGCAGCTGT TAACTTTGGA CAGTATCCAT ATGGAGGTTT AATCCTAAAC AGGCCAACCA TTAGCAGGAG ATTCATGCCT GAGAAAGGGT CTGCTGAGTA TGCAGCTTTG GCTAAGAACC CTGAGAAGGA GTTTTTGAAG ACTATTACTG GCAAGAAGGA GACCCTCATT GACCTTACAA TTATAGAAAT CTTGTCAAGG CACACATCTG ATGAGTTCTA CCTTGGGGAG AGAGATGGTG GTGACTATTG GACTTCTGAT GCTGGTCCAC TAGAGGCCTT CAAGAGGTTT GGAAAGAAGC TTCAAGAGAT TGAACAAAAG CTTATACAGA AGAACAAGGA CGAGACGTTG AGAAACCGCA GTGGGCCGGC TAAAATGCCT TACACTTTGC TCTATCCTTC TAGTGAGGAG GGATTGACTT TCAGAGGAAT TCCCAACAGT ATCTCCATCT AGGGGCTCTA TGGTTGTTGC AGTACTTTCC CTGGTTTTCA ATAAAAGTGA ATAGAAGGAA TATGATTCCT TCATCTACCT ATGTATGGTT AAGTTATGTA TTTGGGGTGA TTGAATGTTC TCGTGTTTGC TGTTAAGTGT TAATGAAGAA ATTCTGTTTG TATAAATTGC TGTTAAGTGT TCTCGCGCAT GTTTCAACAA TGATAGCTGG CATGGCAAAG TGCAAACTAG AATAAATCAT CATCTTATGT AAACGATTTC CATTACAGCA AGGGTTGTCC TTAAAATTTC ATTGCCCGTG TATTCATATT TTTGCATTGC CTCAAAGTTA CCACTTCCTC ACCTTAGTAT CTATTTTCTT AAAG'; 
return $element;
}


