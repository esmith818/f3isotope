Some notes on how to use this module.

(0) Make sure cron is enabled and working. Date Reminder depends on cron.

(1) Install and enable in the usual way.

(2) Go to Administer>Settings>Date reminder. There are four settings you can
    control:

  - Cron frequency. Tell the module how often cron runs. This fine-tunes
    when reminders are actually sent.

  - Max reminders.  A user can request multiple reminders at different
    times for the same event. Specify the maximum number of reminders
    one user can schedule for a node.  (A user might want a reminder,
    say, a week before an event, a day before AND a couple of hours
    before.)

  - How many new reminders a user can add in a single batch.

  - Are the user's reminders displayed on each node as a tab or as
    an expandable field set in the node listing?

  - How long should reminders be kept after the last date occurrence has passed?
    (Keeping reminders around means they'll be in force again if the date
    of the node is changed.

  - You can also configure the values that will be displayed as allowed
    lead times for reminders. Keep in mind that lead times shorter than
    your cron frequency won't work.


(3) Configure permissions

Note: Reminders are allowed for authenticated users only.

Otherwise, grant permissions to:

Administer reminders.  This gives permission to see and change other
users' reminders, and to control what reminders can be sent.

Request reminders. Allows user to request reminders.

See other users' reminders. This user can see others' reminders, but
not change them.

Send reminder to arbitrary email address.  Normally, a reminders are sent
only to the user's registered email. This permission allows the user to
specify an arbitrary email address for the reminder.  Use with care.

(4) Configure email templates.

Also on the Date Reminder admin page, use the Email tab to customize the
email that will be sent for reminders. Use tokens.

(5) Enable reminders for content types.

Go to Administer>Content management>Content types. "Edit" a content type
that has at least one date field.  You'll see an expandable group for
"Reminder settings."

You can enable or disable reminders for nodes of this type.  If "Allowed,
off by default," anyone who can edit a node can enable reminders for
that node, but this needs to be done for each node.  If "Allowed, on by
default," reminders will be enabled whenever a new node is created.

If there are several date fields in the node, you'll need to specify
which is used as the basis for reminders.

(6) Enable reminders for a given node.

Edit the node. Expand "Reminder settings." Select the appropriate option
and save.

(7) Request a reminder.

View the node. Assuming proper permissions, and depending on what you
selected in (2), you'll either see a Reminders tab or an expandable field
in the node. Use the form to request a reminder.  (Select the check box
to get an immediate trial reminder message.)

(8) Administer

With appropriate privilege, you should be able to see others'
reminders, either in the node's Reminder tab, or under Administer>User
management>Users. And administrator can also remove (but not edit)
other users' reminders.  That may change in the future.



Outstanding issues -

  - If you use one of the administrative options to delete a set of
    reminders, there's no "Are you sure you want to do
    this?" confirmation.  There should be.
