
-- SUMMARY --

The Date Repeat Entity Module complements the Date suite of modules, in
particular the Date Repeat module.  For repeating dates that follow a
recurring rule, this module creates a unique entity for each date instance in
the series.

When an entity that belongs to a repeating date series is updated
or deleted, the user is offered the choice of updating/deleting the current
entity, all current and future entities or all entities in the series.

If the recurring rule for a date series is changed significantly (and this
can be controlled through a hook - see Customization section below) the existing
entities are replaced by a new set of entities that is generated to represent
the new series.  In this case the module provides a warning to the user if
referenced entities, associated with the current date entity, may be affected.

Note: The module includes code that specifically targets 'node' entities.
However, many of the functions have been written generically so that they
will work with other entity types.

-- REQUIREMENTS --

The following modules must be installed and enabled for this module to work:

* Date, Date API, Date Repeat, Date Repeat Field (all part of Date module)
* Entity
* Replicate
* UUID

-- INSTALLATION --

* Install as usual.  For more information see
 https://drupal.org/documentation/install/modules-themes/modules-7

-- CONFIGURATION --

* Go to admin/config/date/date_repeat_entity and check each content type that
  you want to enable for this module.

* The module will create two additional fields for each enabled content type.

* If necessary, add a date field to each content type.  (Make sure the date
  field is configured as a repeating date with an end date.)

-- CUSTOMIZATION --

* When a date series is changed it may be  significant enough that the existing
  entities in the series should be deleted and replaced by a new series.  This
  module provides HOOK_repeat_entity_repeating_date_has_changed that gives other
  module developers the ability to determine when the change is significant such
  that the series is replaced.

* If a date series that is changed and the change is not significant to generate
  a new series the existing entities representing each date in the series are
  updated.  A separate HOOK_repeating_date_update is provided to enable other
  module developers to determine which properties and fields of each entity
  should be updated.

-- CONTACT --

Current maintainers:
* Simon Shutter (eft) - https://drupal.org/user/458578
* Tom Metzger (tmetzger) - https://drupal.org/user/423974

This project has been sponsored by:

* Groupanizer Technology Services

  We create websites and online tools that help choirs, choruses and music
  groups manage and organize members, music and more while creating a
  captivating public presence.  Visit http://www.groupanizer.com/ for more
  information.
