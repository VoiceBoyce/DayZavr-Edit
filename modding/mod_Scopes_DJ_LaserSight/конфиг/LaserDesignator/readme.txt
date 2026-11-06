 
(First start its full random settings for laser colors. File readme created only one time.)
 
WorkNonRaised - mode of the laser, in case of 1, the laser works always
when it is in the player's hands. In case of 0, the laser works only when the
weapon is raised.
 
ScreenCenterMode - vanilla shooting mechanics implies a very casual way of
shooting - in the center of the player's screen, although it can work
realistically enough along the barrel of the weapon, but in this case, when
shooting at close range, the accuracy of the hit is shifted from the center of
the sight on the screen.
In case 1, the laser tends to match the hit point in the center of the screen.
In the case of 0, the laser corresponds to an accurate shot from the barrel of the
weapon, but may not correspond to the final point of impact. (c) because Bohemia.
~1: maybe some strange...
 
addition of laser colors.
dotptc = particle that appears at the end of the laser, available R, G, B, W. 
In case alpha < 1 - the laser beam may not be visible in optical sights.
In cases where alpha<0.5 the beam may not be visible in NVGs.
In cases where alpha or all colors == 0, the beam is no longer visible at all,
but still works, AND IN THAT case, the dot at the end of the beam still works,
which allows the beam to be hidden completely without loss of performance.
 
Button for change mode: L by default, in settings controls > combat.
 
MAX RANGE = ~100m. if longer - game broke lods loading and incorrect create beam for another players.
 
by @Rico_Magic
