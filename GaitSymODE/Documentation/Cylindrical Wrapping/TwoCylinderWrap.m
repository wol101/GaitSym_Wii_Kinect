% function to wrap a line around two parallel cylinders

% the cylinders are assumed to have their axes along the z axis and the wrapping is according
% to the right hand rule

% the coordinate system is right handed too

function [wrapOK, originForce, insertionForce, cylinderForce1, cylinderForcePosition1, cylinderForce2, cylinderForcePosition2, pathLength, pathCoordinates] = ...
   TwoCylinderWrap(origin, insertion, cylinderPosition1, radius1, cylinderPosition2, radius2, tension, nPointsPerCylinderArc, maxAngle)

% this is the special case looking down the axis of the cylinder (i.e. xy plane)
% this is standard tangent to a circle stuff

% using letters as defined in TwoCylinderDiagram
% set knowns to match diagram

O = origin;
I = insertion;
C = cylinderPosition1;
D = cylinderPosition2;
r = radius1;
s = radius2;

small_angle = 1e-10;

% origin to first cylinder 
[E1, E2, number_of_tangents] = FindTangents(C, r, O);
if (number_of_tangents == 0)
    wrapOK = -1;
    return
end

% insertion to second cylinder
[H1, H2, number_of_tangents] = FindTangents(D, s, I);
if (number_of_tangents == 0)
    wrapOK = -1;
    return
end

% now find line between cylinders
[F1, G1, F2, G2, inner1_p1, inner1_p2, inner2_p1, inner2_p2, number_of_tangents] = FindCircleCircleTangents(C, r, D, s);
wrapOK = number_of_tangents;

% now calculate the planar path length
cyl1_start_angle = atan2(E2.y - C.y, E2.x - C.x);
cyl1_end_angle = atan2(F2.y - C.y, F2.x - C.x);
cyl1_theta = cyl1_end_angle - cyl1_start_angle;
if (cyl1_theta < 0); cyl1_theta = cyl1_theta + 2 * pi(); end
cyl2_start_angle = atan2(G2.y - D.y, G2.x - D.x);
cyl2_end_angle = atan2(H1.y - D.y, H1.x - D.x);
cyl2_theta = cyl2_end_angle - cyl2_start_angle;
if (cyl2_theta < 0); cyl2_theta = cyl2_theta + 2 * pi(); end

% use maxAngle to decide whether wrapping is appropriate
% values between pi and 2pi are sensible

if (cyl1_theta < maxAngle && cyl2_theta < maxAngle && cyl1_theta > small_angle && cyl2_theta > small_angle) % the double wrap case
    
    wrapOK = 1;

    l1 = vector_distance2d(O, E2);
    l2 = cyl1_theta * r;
    l3 = vector_distance2d(F2, G2);
    l4 = cyl2_theta * s;
    l5 = vector_distance2d(H1, I);
    
    planar_path_length = l1 + l2 + l3 + l4 + l5;
    delta_Z = I.z - O.z;
    
    E2.z = O.z + delta_Z * l1 / planar_path_length;
    F2.z = O.z + delta_Z * (l1 + l2) / planar_path_length;
    G2.z = O.z + delta_Z * (l1 + l2 + l3) / planar_path_length;
    H1.z = O.z + delta_Z * (l1 + l2 + l3 + l4) / planar_path_length;
    
    originForce = vector_with_magnitude(O, E2, tension);
    insertionForce = vector_with_magnitude(I, H1, tension);
    
    betweenForce = vector_with_magnitude(F2, G2, tension);
    
    cylinderForce1.x = betweenForce.x - originForce.x;
    cylinderForce1.y = betweenForce.y - originForce.y;
    cylinderForce1.z = betweenForce.z - originForce.z;
    cylinderForcePosition1.x = cylinderPosition1.x;
    cylinderForcePosition1.y = cylinderPosition1.y;
    cylinderForcePosition1.z = (E2.z + F2.z) / 2;
    
    cylinderForce2.x = -betweenForce.x - insertionForce.x;
    cylinderForce2.y = -betweenForce.y - insertionForce.y;
    cylinderForce2.z = -betweenForce.z - insertionForce.z;
    cylinderForcePosition2.x = cylinderPosition2.x;
    cylinderForcePosition2.y = cylinderPosition2.y;
    cylinderForcePosition2.z = (G2.z + H1.z) / 2;
    
    pathLength = sqrt(delta_Z * delta_Z + planar_path_length * planar_path_length);
    
    pathCoordinates = zeros(6 + (nPointsPerCylinderArc - 1), 3);
    i = 1;
    pathCoordinates(i, 1) = O.x;
    pathCoordinates(i, 2) = O.y;
    pathCoordinates(i, 3) = O.z;
    i = i + 1;
    pathCoordinates(i, 1) = E2.x;
    pathCoordinates(i, 2) = E2.y;
    pathCoordinates(i, 3) = E2.z;
    i = i + 1;
    
    % now fill in the missing bits of the 1st circle
    cyl1_del_theta = cyl1_theta / nPointsPerCylinderArc;
    theta = cyl1_start_angle;
    for j = 1 : nPointsPerCylinderArc - 1
        theta = theta + cyl1_del_theta;
        pathCoordinates(i, 1) = C.x + r * cos(theta);
        pathCoordinates(i, 2) = C.y + r * sin(theta);
        pathCoordinates(i, 3) = O.z + delta_Z * (l1 + (j/nPointsPerCylinderArc) * l2) / planar_path_length;
        i = i + 1;
    end
    
    pathCoordinates(i, 1) = F2.x;
    pathCoordinates(i, 2) = F2.y;
    pathCoordinates(i, 3) = F2.z;
    i = i + 1;
    pathCoordinates(i, 1) = G2.x;
    pathCoordinates(i, 2) = G2.y;
    pathCoordinates(i, 3) = G2.z;
    i = i + 1;
    
    % now fill in the missing bits of the 2nd circle
    cyl2_del_theta = cyl2_theta / nPointsPerCylinderArc;
    theta = cyl2_start_angle;
    for j = 1 : nPointsPerCylinderArc - 1
        theta = theta + cyl2_del_theta;
        pathCoordinates(i, 1) = D.x + s * cos(theta);
        pathCoordinates(i, 2) = D.y + s * sin(theta);
        pathCoordinates(i, 3) = O.z + delta_Z * (l1 + l2 + l3 + (j/nPointsPerCylinderArc) * l4) / planar_path_length;
        i = i + 1;
    end
    
    pathCoordinates(i, 1) = H1.x;
    pathCoordinates(i, 2) = H1.y;
    pathCoordinates(i, 3) = H1.z;
    i = i + 1;
    pathCoordinates(i, 1) = I.x;
    pathCoordinates(i, 2) = I.y;
    pathCoordinates(i, 3) = I.z;
    
    return
end

if (cyl1_theta < maxAngle && cyl1_theta > small_angle) % try cyl 1 wrapping
    
    % insertion to first cylinder
    [K1, K2, number_of_tangents] = FindTangents(C, r, I);
    if (number_of_tangents == 0)
        wrapOK = -1;
        return
    end
    
    % calculate new angle
    cyl1_start_angle = atan2(E2.y - C.y, E2.x - C.x);
    cyl1_end_angle = atan2(K1.y - C.y, K1.x - C.x);
    cyl1_theta = cyl1_end_angle - cyl1_start_angle;
    if (cyl1_theta < 0); cyl1_theta = cyl1_theta + 2 * pi(); end

    if (cyl1_theta < maxAngle)
        
        wrapOK = 2;
        
        l1 = vector_distance2d(O, E2);
        l2 = cyl1_theta * r;
        l3 = vector_distance2d(K1, I);
    
        planar_path_length = l1 + l2 + l3;
        delta_Z = I.z - O.z;
    
        E2.z = O.z + delta_Z * l1 / planar_path_length;
    	K1.z = O.z + delta_Z * (l1 + l2) / planar_path_length;
    
        originForce = vector_with_magnitude(O, E2, tension);
        insertionForce = vector_with_magnitude(I, K1, tension);
    
        cylinderForce1.x = -insertionForce.x - originForce.x;
        cylinderForce1.y = -insertionForce.y - originForce.y;
        cylinderForce1.z = -insertionForce.z - originForce.z;
        cylinderForcePosition1.x = cylinderPosition1.x;
        cylinderForcePosition1.y = cylinderPosition1.y;
        cylinderForcePosition1.z = (E2.z + K1.z) / 2;
        
        cylinderForce2.x = 0;
        cylinderForce2.y = 0;
        cylinderForce2.z = 0;
        cylinderForcePosition2.x = 0;
        cylinderForcePosition2.y = 0;
        cylinderForcePosition2.z = 0;
        
        pathLength = sqrt(delta_Z * delta_Z + planar_path_length * planar_path_length);
        
        pathCoordinates = zeros(4 + (nPointsPerCylinderArc - 1), 3);
        i = 1;
        pathCoordinates(i, 1) = O.x;
        pathCoordinates(i, 2) = O.y;
        pathCoordinates(i, 3) = O.z;
        i = i + 1;
        pathCoordinates(i, 1) = E2.x;
        pathCoordinates(i, 2) = E2.y;
        pathCoordinates(i, 3) = E2.z;
        i = i + 1;
        
        % now fill in the missing bits of the 1st circle
        cyl1_del_theta = cyl1_theta / nPointsPerCylinderArc;
        theta = cyl1_start_angle;
        for j = 1 : nPointsPerCylinderArc - 1
            theta = theta + cyl1_del_theta;
            pathCoordinates(i, 1) = C.x + r * cos(theta);
            pathCoordinates(i, 2) = C.y + r * sin(theta);
            pathCoordinates(i, 3) = O.z + delta_Z * (l1 + (j/nPointsPerCylinderArc) * l2) / planar_path_length;
            i = i + 1;
        end
        
        pathCoordinates(i, 1) = K1.x;
        pathCoordinates(i, 2) = K1.y;
        pathCoordinates(i, 3) = K1.z;
        i = i + 1;
        pathCoordinates(i, 1) = I.x;
        pathCoordinates(i, 2) = I.y;
        pathCoordinates(i, 3) = I.z;
        
        return
    end
end
    
if (cyl2_theta < maxAngle && cyl2_theta > small_angle) % try cyl 2 wrapping
    
    % insertion to first cylinder
    [J1, J2, number_of_tangents] = FindTangents(D, s, O);
    if (number_of_tangents == 0)
        wrapOK = -1;
        return
    end
    
    % calculate new angle
    cyl2_start_angle = atan2(J2.y - D.y, J2.x - D.x);
    cyl2_end_angle = atan2(H1.y - D.y, H1.x - D.x);
    cyl2_theta = cyl2_end_angle - cyl2_start_angle;
    if (cyl2_theta < 0); cyl2_theta = cyl2_theta + 2 * pi(); end

    if (cyl2_theta < maxAngle)
        
        wrapOK = 3;
        
        l1 = vector_distance2d(O, J2);
        l2 = cyl2_theta * s;
        l3 = vector_distance2d(H1, I);
    
        planar_path_length = l1 + l2 + l3;
        delta_Z = I.z - O.z;
    
        J2.z = O.z + delta_Z * l1 / planar_path_length;
    	H1.z = O.z + delta_Z * (l1 + l2) / planar_path_length;
    
        originForce = vector_with_magnitude(O, J2, tension);
        insertionForce = vector_with_magnitude(I, H1, tension);
    
        cylinderForce2.x = -insertionForce.x - originForce.x;
        cylinderForce2.y = -insertionForce.y - originForce.y;
        cylinderForce2.z = -insertionForce.z - originForce.z;
        cylinderForcePosition2.x = cylinderPosition2.x;
        cylinderForcePosition2.y = cylinderPosition2.y;
        cylinderForcePosition2.z = (J2.z + H1.z) / 2;
        
        cylinderForce1.x = 0;
        cylinderForce1.y = 0;
        cylinderForce1.z = 0;
        cylinderForcePosition1.x = 0;
        cylinderForcePosition1.y = 0;
        cylinderForcePosition1.z = 0;
        
        pathLength = sqrt(delta_Z * delta_Z + planar_path_length * planar_path_length);
        
        pathCoordinates = zeros(4 + (nPointsPerCylinderArc - 1), 3);
        i = 1;
        pathCoordinates(i, 1) = O.x;
        pathCoordinates(i, 2) = O.y;
        pathCoordinates(i, 3) = O.z;
        i = i + 1;
        pathCoordinates(i, 1) = J2.x;
        pathCoordinates(i, 2) = J2.y;
        pathCoordinates(i, 3) = J2.z;
        i = i + 1;
        
        % now fill in the missing bits of the 1st circle
        cyl2_del_theta = cyl2_theta / nPointsPerCylinderArc;
        theta = cyl2_start_angle;
        for j = 1 : nPointsPerCylinderArc - 1
            theta = theta + cyl2_del_theta;
            pathCoordinates(i, 1) = D.x + s * cos(theta);
            pathCoordinates(i, 2) = D.y + s * sin(theta);
            pathCoordinates(i, 3) = O.z + delta_Z * (l1 + (j/nPointsPerCylinderArc) * l2) / planar_path_length;
            i = i + 1;
        end
        
        pathCoordinates(i, 1) = H1.x;
        pathCoordinates(i, 2) = H1.y;
        pathCoordinates(i, 3) = H1.z;
        i = i + 1;
        pathCoordinates(i, 1) = I.x;
        pathCoordinates(i, 2) = I.y;
        pathCoordinates(i, 3) = I.z;
        
        return
    end
end

% if we get here then no wrapping is possible

wrapOK = 0;

pathLength = vector_distance3d(O, I);

originForce = vector_with_magnitude(O, I, tension);
insertionForce.x = -originForce.x;
insertionForce.y = -originForce.y;
insertionForce.z = -originForce.z;

cylinderForce1.x = 0;
cylinderForce1.y = 0;
cylinderForce1.z = 0;
cylinderForcePosition1.x = 0;
cylinderForcePosition1.y = 0;
cylinderForcePosition1.z = 0;
cylinderForce2.x = 0;
cylinderForce2.y = 0;
cylinderForce2.z = 0;
cylinderForcePosition2.x = 0;
cylinderForcePosition2.y = 0;
cylinderForcePosition2.z = 0;

pathCoordinates = zeros(2, 3);
i = 1;
pathCoordinates(i, 1) = O.x;
pathCoordinates(i, 2) = O.y;
pathCoordinates(i, 3) = O.z;
i = i + 1;
pathCoordinates(i, 1) = I.x;
pathCoordinates(i, 2) = I.y;
pathCoordinates(i, 3) = I.z;

return

% From http://www.vb-helper.com/howto_net_circle_circle_tangents.html
% Find the tangent points for these two circles.
% Return the number of tangents: 4, 2, or 0.
function [outer1_p1, outer1_p2, outer2_p1, outer2_p2, inner1_p1, inner1_p2, inner2_p1, inner2_p2, number_of_tangents] = FindCircleCircleTangents(c1, radius1, c2, radius2) 

outer1_p1.x = 0; outer1_p1.y = 0;
outer1_p2.x = 0; outer1_p2.y = 0;
outer2_p1.x = 0; outer2_p1.y = 0;
outer2_p2.x = 0; outer2_p2.y = 0;
inner1_p1.x = 0; inner1_p1.y = 0;
inner1_p2.x = 0; inner1_p2.y = 0;
inner2_p1.x = 0; inner2_p1.y = 0;
inner2_p2.x = 0; inner2_p2.y = 0;

% Make sure radius1 <= radius2.
if (radius1 > radius2) 
    % Call this method switching the circles.
    [outer2_p2, outer2_p1, outer1_p2, outer1_p1, inner2_p2, inner2_p1, inner1_p2, inner1_p1 number_of_tangents] = ...
        FindCircleCircleTangents(c2, radius2, c1, radius1);
    return
end

% ***************************
% * Find the outer tangents *
% ***************************
radius2a = radius2 - radius1;
[outer1_p2, outer2_p2, number_of_tangents] = FindTangents(c2, radius2a, c1);
if (number_of_tangents == 0)
	% There are no tangents.
    number_of_tangents = 0;
    return
end

% Get the vector perpendicular to the
% first tangent with length radius1.
v1x = -(outer1_p2.y - c1.y);
v1y = outer1_p2.x - c1.x;
v1_length = sqrt(v1x * v1x + v1y * v1y);
v1x = v1x * radius1 / v1_length;
v1y = v1y * radius1 / v1_length;
% Offset the tangent vector's points.
outer1_p1.x = c1.x + v1x;
outer1_p1.y = c1.y + v1y;
outer1_p2.x = outer1_p2.x + v1x;
outer1_p2.y = outer1_p2.y + v1y;

% Get the vector perpendicular to the
% second tangent with length radius1.
v2x = outer2_p2.y - c1.y;
v2y = -(outer2_p2.x - c1.x);
v2_length = sqrt(v2x * v2x + v2y * v2y);
v2x = v2x * radius1 / v2_length;
v2y = v2y * radius1 / v2_length;
% Offset the tangent vector's points.
outer2_p1.x = c1.x + v2x;
outer2_p1.y = c1.y + v2y;
outer2_p2.x = outer2_p2.x + v2x;
outer2_p2.y = outer2_p2.y + v2y;

% If the circles intersect, then there are no inner
% tangents.
dx = c2.x - c1.x;
dy = c2.y - c1.y;
dist = sqrt(dx * dx + dy * dy);
if (dist <= radius1 + radius2)
    number_of_tangents = 2;
    return
end

% ***************************
% * Find the inner tangents *
% ***************************
radius1a = radius1 + radius2;
[inner1_p2, inner2_p2, number_of_tangents] = FindTangents(c1, radius1a, c2);

% Get the vector perpendicular to the
% first tangent with length radius2.
v1x = inner1_p2.y - c2.y;
v1y = -(inner1_p2.x - c2.x);
v1_length = sqrt(v1x * v1x + v1y * v1y);
v1x = v1x * radius2 / v1_length;
v1y = v1y * radius2 / v1_length;
% Offset the tangent vector's points.
inner1_p1.x = c2.x + v1x;
inner1_p1.y = c2.y + v1y;
inner1_p2.x = inner1_p2.x + v1x;
inner1_p2.y = inner1_p2.y + v1y;

% Get the vector perpendicular to the
% second tangent with length radius2.
v2x = -(inner2_p2.y - c2.y);
v2y = inner2_p2.x - c2.x;
v2_length = sqrt(v2x * v2x + v2y * v2y);
v2x = v2x * radius2 / v2_length;
v2y = v2y * radius2 / v2_length;
% Offset the tangent vector's points.
inner2_p1.x = c2.x + v2x;
inner2_p1.y = c2.y + v2y;
inner2_p2.x = inner2_p2.x + v2x;
inner2_p2.y = inner2_p2.y + v2y;

number_of_tangents = 4;
return


% Adapted from http://www.vb-helper.com/howto_net_find_circle_tangents.html
% Find the tangent points for this circle and external
% point.
% Return the number of tangents: 2, or 0.
function [pt1, pt2, number_of_tangents] = FindTangents(center, radius, external_point)

pt1.x = 0; pt1.y = 0;
pt2.x = 0; pt2.y = 0;

% Find the distance squared from the
% external point to the circle's center.
dx = center.x - external_point.x;
dy = center.y - external_point.y;
D_squared = dx * dx + dy * dy;
if (D_squared < radius * radius) 
    number_of_tangents = 0;
    return
end

% Find the distance from the external point
% to the tangent points.
L = sqrt(D_squared - radius * radius);

% Find the points of intersection between
% the original circle and the circle with
% center external_point and radius dist.

[pt1, pt2, number_of_intersections] = FindCircleCircleIntersections(center.x, center.y, radius, external_point.x, external_point.y, L);
number_of_tangents = 2;
return

% http://www.vb-helper.com/howto_net_circle_circle_intersection.html
% Find the points where the two circles intersect.
function [intersection1, intersection2, number_of_intersections] = FindCircleCircleIntersections(cx0, cy0, radius0, cx1, cy1, radius1)

intersection1.x = 0; intersection1.y = 0;
intersection2.x = 0; intersection2.y = 0;

% Find the distance between the centers.
dx = cx0 - cx1;
dy = cy0 - cy1;
dist = sqrt(dx * dx + dy * dy);

% See how many solutions there are.
if (dist > radius0 + radius1) 
	% No solutions, the circles are too far apart.
    number_of_intersections = 0;
    return
end
if (dist < abs(radius0 - radius1)) Then
    % No solutions, one circle contains the other.
    number_of_intersections = 0;
    return
end
if ((dist == 0) && (radius0 == radius1))
    % No solutions, the circles coincide.
    number_of_intersections = 0;
    return
end

% Find a and h.
a = (radius0 * radius0 - radius1 * radius1 + dist * dist) / (2 * dist);
h = sqrt(radius0 * radius0 - a * a);

% Find P2.
cx2 = cx0 + a * (cx1 - cx0) / dist;
cy2 = cy0 + a * (cy1 - cy0) / dist;

% Get the points P3.
intersection1.x = cx2 + h * (cy1 - cy0) / dist;
intersection1.y = cy2 - h * (cx1 - cx0) / dist;
intersection2.x = cx2 - h * (cy1 - cy0) / dist;
intersection2.y = cy2 + h * (cx1 - cx0) / dist;

% See if we have 1 or 2 solutions.
if (dist == radius0 + radius1)
    number_of_intersections = 1;
else
    number_of_intersections = 2;
end

return

% calculate the 2D length of a vector
function l = vector_distance2d(v1, v2)

l = sqrt((v2.x - v1.x) * (v2.x - v1.x) + (v2.y - v1.y) * (v2.y - v1.y));

return

% calculate the 3D length of a vector
function l = vector_distance3d(v1, v2)

l = sqrt((v2.x - v1.x) * (v2.x - v1.x) + (v2.y - v1.y) * (v2.y - v1.y) + (v2.z - v1.z) * (v2.z - v1.z));

return

% return the vector from v1 to v2
function v = vector_from(v1, v2)
v.x = v2.x - v1.x;
v.y = v2.y - v1.y;
v.z = v2.z - v1.z;
return

% return the vector in the direction of v1 to v2 with magnitude specified
function v = vector_with_magnitude(v1, v2, magnitude)
del_v = vector_from(v1, v2);
mag = vector_distance3d(v1, v2);
v.x = magnitude * del_v.x / mag;
v.y = magnitude * del_v.y / mag;
v.z = magnitude * del_v.z / mag;
return




