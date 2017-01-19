README.txt
==========

IP Ranges is a module that let's you completely ban both single IP-addresses as well as full ranges from your site. The ban is triggered already at the bootstrap phase, so youcan get rid of unwanted visitors as early as possible without wasting server resources.

You can also define whitelists that override blacklists, both single and ranged.
The UI is similar to core ip-ban, so you will feel like home immediately.


INSTALLATION
=============

Just enable the module as usual.


USAGE
============
After enabling the module, go to admin/config/people/ip-ranges to find form with three elements: 
"IP range start / Single IP-address"
"IP range end"
"List type"

Two first two fields take an IP-Address in the form of "100.100.100.100". If the second field is filled, 
they be treated as a range. If you leave it empty, the value from the first field is only used.
(This is currently the only allowed range form, other types like bitmasks may come at later stage).

Type can be either "blacklist" or "whitelist",
where blacklisted IP's are denied from the site, and whitelisted are allowed.
Whitelists always override blacklists.


DEVELOPERS
============

While there is no exactly "API" available, there are some functions you can use in your code if needed:

To ban IP-Address / -Range:

ip_ranges_write_record($ip, $type, $bid='');

Where both ip and type are entered just like from the ui. $bid, or ban id, is optional and can be used to update existing record.

To retrieve list of ban records:

ip_ranges_get_ip_list($type='');

Again, type is either "blacklist", "whitelist" or empty. Return will be array of IP's of requested type. 
Ranges will be like this: "100.100.100.100-100.100.100.200".

