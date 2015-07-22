Mother May I

This is a module intended for sites that only expect logins from
a limited population such as members of a local organization. It's
intended to block access from hackers who try to create accounts
so they can post for their own nefarious purposes.

The site administrator can define a "secret word" and hint text. The
word is something that should be obvious to someone who's a member
of the target audience, but not a hacker in China, the former USSR,
or other hotbeds of spam attacks.

If a user trying to create an account doesn't enter the secret word,
no account is created.


Overview
Most Drupal sites that allow user login are plagued by account requests 
from abusers and bots whose only intent is to post content to the site 
for nefarious purposes. While Drupal supplies tools that are effective 
at keeping them out (Captcha, for example, and administration approval 
of new accounts), there's still some level of overhead for administrators.

Mother May I provides a simple to use extra hurdle to reduce the bother 
of spamming account requests. It's primarily useful to sites with a 
limited target audience (at least for authenticated accounts) where 
valid users have some side information about the group. The site 
administrator can define a site-specific "secret word." Anyone requesting 
an account must enter the secret word before even a temporary account is 
created.

It's highly recommended that sites not depend solely on Mother May I to vet 
account requests, but this is an easy way to block spam requests from abusers 
that manage to get past the existing filters.


Features
The site administrator specifies a secret word. A user requesting a new 
account must enter the secret word before account creation can proceed.

The administrator can also enter a "hint" block. The hint should be 
descriptive enough that a valid site user (for example, a member of the 
organization) can easily figure out the word from the hint, but someone 
outside the organization cannot. It's up to the administrator to decide how 
cryptic s/he wants to be.

If no secret word is defined, Mother May I doesn't impact the account 
request process.

Known Issues
    I know you could probably do something similar to this using Rules, 
      but this is easy to install and maintain.
    It might be nice to keep a log of rejected requests, but providing an 
      interface to report them and then clean them up is more work than just 
       checking the secret word.

Administration and Use
On the Mother May I administration screen, you can set four bits of information:

    - The secret word. This needs to be alphanumeric. If no secret word is 
      specified, the users requesting accounts won't be asked for one.
    - The password hint. If entered, this text will be displayed above the 
      secret word entry box on the account register page.
    - Password hint filter. This says what kind of filtering (HTML, etc.) 
      to use when displaying the hint.
    - Form weight. The default should be fine, but this lets you move the 
      "secret word" block up and down in the account registration form. 

If a secret word and optional hint are supplied, users will see a required 
"Enter the secret word" box on the account request page.
