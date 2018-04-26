var WysiwygPluginSmileyInstanceId = null;
Drupal.wysiwyg.plugins['smiley_extra'] = {
  attach: function(content, settings, instanceId) {
    return content;
  },

  invoke: function(data, settings, instanceId) {
    WysiwygPluginSmileyInstanceId = instanceId;
    var el = document.getElementById('smiley-wysiwyg');

    if (el != undefined) {
      document.body.removeChild(el);
    }

    var url = Drupal.settings.basePath + 'index.php?q=smiley_wysiwyg/dialog';
    var ifr = '<iframe src="' + url + '" style="width:600px;height:500px;border:0"></iframe>';

    var div = document.createElement('div');
    div.style.width = '600px';
    div.style.padding = '0px';
    div.style.height = '400px';
    div.style.position = 'fixed';
    div.style.left = '50%';
    div.style.top = '50%';
    div.style.marginLeft = '-300px';
    div.style.marginTop = '-200px';
    div.style.backgroundColor = '#FFFFFF';
    div.style.zIndex = '1000001';
    div.style.borderWidth = '5px';
    div.style.borderStyle = 'solid';
    div.style.borderColor = '#EDEDED';
    div.id = 'smiley-wysiwyg';
    div.innerHTML = ifr;

    document.body.appendChild(div);
  }
};

function WysiwygPluginSmileyClose() {
  var el = document.getElementById('smiley-wysiwyg');
  if (el != undefined) {
    document.body.removeChild(el);
  }
}
