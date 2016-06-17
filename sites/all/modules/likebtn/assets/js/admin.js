/**
 * Test synchronization.
 */
function testSync(ajaxurl)
{
    jQuery(".likebtn_test_sync_container:first img").show();
    jQuery(".likebtn_test_sync_container:first .likebtn_test_sync_message").hide();

    jQuery.ajax({
        type: 'POST',
        dataType: "json",
        url: ajaxurl,
        data: {
            likebtn_account_email: jQuery("#edit-likebtn-account-data-email").val(),
            likebtn_account_api_key: jQuery("#edit-likebtn-account-data-api-key").val()
        },
        success: function(response) {
            var result_text = '';
            if (typeof(response.result_text) != "undefined") {
                result_text = response.result_text;
            }
            jQuery(".likebtn_test_sync_message:first").text(result_text).show();
            if (typeof(response.result) == "undefined" || response.result != "success") {
                jQuery(".likebtn_test_sync_message").css('color', 'red');
                if (typeof(response.message) != "undefined") {
                    var text = jQuery(".likebtn_test_sync_message").text() + ': ' + response.message;
                    jQuery(".likebtn_test_sync_message").text(text);
                }
            } else {
                jQuery(".likebtn_test_sync_message").css('color', 'green');
            }
            jQuery(".likebtn_test_sync_container:first img").hide();

        },
        error: function(response) {
            jQuery(".likebtn_test_sync_message").text('Error').css('color', 'red').show();
            jQuery(".likebtn_test_sync_container:first img").hide();
        }
    });

    return false;
}
