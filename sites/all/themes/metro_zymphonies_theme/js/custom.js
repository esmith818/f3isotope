
/* --------------------------------------------- 

* Filename:     custom.js
* Version:      1.0.0 (2014-11-09)
* Website:      http://www.zymphonies.com
* Description:  System Styles
* Author:       Zymphonies Dev Team
                info@zymphonies.com

-----------------------------------------------*/

jQuery(document).ready(function($) {

  $('.nav-toggle').click(function() {
    $('#main-menu div ul:first-child').slideToggle(250);
    return false;
  });

  if( ($(window).width() > 640) || ($(document).width() > 640) ) {
      $('#main-menu li').mouseenter(function() {
        $(this).children('ul').css('display', 'none').stop(true, true).slideToggle(250).css('display', 'block').children('ul').css('display', 'none');
      });
      $('#main-menu li').mouseleave(function() {
        $(this).children('ul').stop(true, true).fadeOut(250).css('display', 'block');
      })
        } else {
    $('#main-menu li').each(function() {
      if($(this).children('ul').length)
        $(this).append('<span class="drop-down-toggle"><span class="drop-down-arrow"></span></span>');
    });
    $('.drop-down-toggle').click(function() {
      $(this).parent().children('ul').slideToggle(250);
    });
  }

  $('.social-icons li').each(function(){
    var url = $(this).find('a').attr('href');
    if(url == ''){
     $(this).hide();
    }
  });

  $('.field-name-field-download-pdf a').text('Download Pdf');

});