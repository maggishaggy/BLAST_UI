<script>
     window.onload = function() {
    if(!window.location.hash) {
        window.location = window.location + '#loaded';
        window.location.reload();
    }
}

</script>
<?php

/**
 * Display the results of a BLAST job execution
 *
 * Variables Available in this template:
 *   $xml_filename: The full path & filename of XML file containing the BLAST results
 */

// Set ourselves up to do link-out if our blast database is configured to do so.
$linkout = FALSE;

if ($blastdb->linkout->none === FALSE) {
  $linkout = TRUE;
  $linkout_regex = $blastdb->linkout->regex;
  if (isset($blastdb->linkout->db_id->urlprefix) AND !empty($blastdb->linkout->db_id->urlprefix)) {
    $linkout_urlprefix = $blastdb->linkout->db_id->urlprefix;
  }
  else {
    $linkout = FALSE;
  }
}

// Handle no hits. This following array will hold the names of all query
// sequences which didn't have any hits.
$query_with_no_hits = array();

// Furthermore, if no query sequences have hits we don't want to bother listing
// them all but just want to give a single, all-include "No Results" message.
$no_hits = TRUE;

?>

<!-- JQuery controlling display of the alignment information (hidden by default) -->
<script type="text/javascript">
  $(document).ready(function(){

    // Hide the alignment rows in the table
    // (ie: all rows not labelled with the class "result-summary" which contains the tabular
    // summary of the hit)
    $("#blast_report tr:not(.result-summary)").hide();
    $("#blast_report tr:first-child").show();

    // When a results summary row is clicked then show the next row in the table
    // which should be corresponding the alignment information
    $("#blast_report tr.result-summary").click(function(){
      $(this).next("tr").toggle();
      $(this).find(".arrow").toggleClass("up");
    });
  });
</script>

<style>
.no-hits-message {
  color: red;
  font-style: italic;
}
</style>

<p><strong>Download</strong>:
  <a href="<?php print '../../' . $html_filename; ?>">Alignment</a>,
  <a href="<?php print '../../' . $tsv_filename; ?>">Tab-Delimited</a>,
  <a href="<?php print '../../' . $xml_filename; ?>">XML</a>
</p>

<p>The following table summarizes the results of your BLAST. To see additional information
about each hit including the alignment, click on that row in the table to expand it.</p>

<?php

// Load the XML file
$xml = simplexml_load_file($xml_filename);

/**
 * We are using the drupal table theme functionality to create this listing
 * @see theme_table() for additional documentation
 */

if ($xml) {
  // Specify the header of the table
  $header = array(
    'number' =>  array('data' => '#', 'class' => array('number')),
    'query' =>  array('data' => 'Query Name', 'class' => array('query')),
    'hit' =>  array('data' => 'Hit Name', 'class' => array('hit')),
    'evalue' =>  array('data' => 'E-Value', 'class' => array('evalue')),
    'arrow-col' =>  array('data' => '', 'class' => array('arrow-col'))
  );

  $rows = array();
  $count = 0;

  // Parse the BLAST XML to generate the rows of the table
  // where each hit results in two rows in the table: 1) A summary of the query/hit and
  // significance and 2) additional information including the alignment
  foreach($xml->{'BlastOutput_iterations'}->children() as $iteration) {
    $children_count = $iteration->{'Iteration_hits'}->children()->count();
    if($children_count != 0) {
      foreach($iteration->{'Iteration_hits'}->children() as $hit) {
        if (is_object($hit)) {
          $count +=1;
          $zebra_class = ($count % 2 == 0) ? 'even' : 'odd';
          $no_hits = FALSE;

          // SUMMARY ROW
          // If the id is of the form gnl|BL_ORD_ID|### then the parseids flag
          // to makeblastdb did a really poor job. In thhis case we want to use
          // the def to provide the original FASTA header.
          $hit_name = (preg_match('/BL_ORD_ID/', $hit->{'Hit_id'})) ? $hit->{'Hit_def'} : $hit->{'Hit_id'};

          // If our BLAST DB is configured to handle link-outs then use the
          // regex & URL prefix provided to create one.
          if ($linkout) {
            if (preg_match($linkout_regex, $hit_name, $linkout_match)) {
              $hit_name = $linkout_match[1];
							
             //  $hit_name_url = l($linkout_urlprefix . $linkout_match[1],
               // array('attributes' => array('target' => '_blank'))
               //  );
            }
          }
					$rounded_evalue = '';
				
          $score = $hit->{'Hit_hsps'}->{'Hsp'}->{'Hsp_score'};
          $evalue = $hit->{'Hit_hsps'}->{'Hsp'}->{'Hsp_evalue'};
			if (strpos($evalue,'e') != false) {
			 $evalue_split = explode('e', $evalue);
			 $rounded_evalue = round($evalue_split[0], 2, PHP_ROUND_HALF_EVEN);				    
				 $rounded_evalue .= 'e' . $evalue_split[1];
			}
			else { 
					$rounded_evalue = $evalue;
			}
				
          $query_name = $iteration->{'Iteration_query-def'};

					// ALIGNMENT ROW (collapsed by default)
          // Process HSPs
      
			    $HSPs = array();
					$track_start = INF;
					$track_end = -1;
					$hsps_range = '';
					
					$gbrowse_url = $blastdb->gbrowse_path;
					if(	$gbrowse_url !=  null) {
						$hit_def = (string) $hit->{'Hit_def'};
						var_dump($hit_def);
						var_dump("\n mtch value: ");
						var_dump(preg_match( '/.*(aradu).*/', $hit_def, $output_array));
						
						if(preg_match('/.*(aradu).*/i', $hit_def) == 1) {
							$gbrowse_url .=   'aradu1.0';
						}
						else if(preg_match('/.*(araip).*/i', $hit_def) == 1) {
							$gbrowse_url .=  'araip1.0';
						} 
						else {
							$gbrowse_url = null;
						}			
					}								

          foreach ($hit->{'Hit_hsps'}->children() as $hsp_xml) {
            $HSPs[] = (array) $hsp_xml;
						$hsps_range .= $hsp_xml->{'Hsp_hit-from'} . '..' . $hsp_xml->{'Hsp_hit-to'} . ',' ;
					
					
						if($track_start > $hsp_xml->{'Hsp_hit-from'}) {
							$track_start = $hsp_xml->{'Hsp_hit-from'} . "";
 						}
  					if($track_end < $hsp_xml->{'Hsp_hit-to'}) {
							$track_end = $hsp_xml->{'Hsp_hit-to'} . "";
 						}
          }
					$range_start = (int) $track_start - 20000;
					$range_end = (int) $track_end + 20000;
				
					if($range_start < 1) 
						 $range_start = 1;
	 		// Link out functionality to GBrowse

				//	$hit_url = $GLOBALS['base_url'] . DIRECTORY_SEPARATOR . 'gbrowse_' . $blastdb->title
					if($gbrowse_url == null) {
						var_dump($gbrowse_url);
						$hit_name_url = $hit_name;
					}
					else {
						$hit_url = 	$GLOBALS['base_url'] . $gbrowse_url . '?' . 'query=' . 'start=' . $range_start . ';' . 'stop=' . $range_end . ';' .
											'ref=' . $hit_name . ';' . 'add=' . $hit_name . '+'	. 'BLAST+' . $iteration->{'Iteration_query-ID'} .
											'+' . $hsps_range . ';h_feat=' . $iteration->{'Iteration_query-ID'} ; //. '@cyan'; 

	
						$hit_name_url = l($hit_name, $hit_url, array('attributes' => array('target' => '_blank')));
					}

          $row = array(
            'data' => array(
              'number' => array('data' => $count, 'class' => array('number')),
              'query' => array('data' => $query_name, 'class' => array('query')),
              'hit' => array('data' => $hit_name_url, 'class' => array('hit')),
              'evalue' => array('data' => $rounded_evalue, 'class' => array('evalue')),
              'arrow-col' => array('data' => '<div class="arrow"></div>', 'class' => array('arrow-col'))
            ),
            'class' => array('result-summary')
          );
          $rows[] = $row;

          

          $row = array(
            'data' => array(
              'number' => '',
              'query' => array(
                'data' => theme('blast_report_alignment_row', array('HSPs' => $HSPs)),
                'colspan' => 4,
              )
            ),
            'class' => array('alignment-row', $zebra_class),
            'no_striping' => TRUE
          );
          $rows[] = $row;

        }// end of if - checks $hit
      } //end of foreach - iteration_hits
    }	// end of if - check for iteration_hits
    else {

      // Currently where the "no results" is added.
      $query_name = $iteration->{'Iteration_query-def'};
      $query_with_no_hits[] = $query_name;

		} // end of else
  }

  if ($no_hits) {
    print '<p class="no-hits-message">No results found.</p>';
  }
  else {
    // We want to warn the user if some of their query sequences had no hits.
    if (!empty($query_with_no_hits)) {
      print '<p class="no-hits-message">Some of your query sequences did not '
      . 'match to the database/template. They are: '
      . implode(', ', $query_with_no_hits) . '.</p>';
    }

    // Actually print the table.
    if (!empty($rows)) {
      print theme('table', array(
        'header' => $header,
        'rows' => $rows,
        'attributes' => array('id' => 'blast_report'),
      ));
    }
  }
}
else {
  drupal_set_title('BLAST: Error Encountered');
  print '<p>We encountered an error and are unable to load your BLAST results.</p>';
}
?>
