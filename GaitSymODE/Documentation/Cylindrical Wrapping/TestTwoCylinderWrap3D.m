function TestTwoCylinderWrap3D()

for i = 1: 2
    figure(i)
    clf('reset');
    set(gcf, 'Color', 'w');
    set(gcf,'Units','pixels');
    set(gcf, 'Position', [100 + 800 * (i - 1), 20, 800, 800], 'Toolbar', 'none');
end


originDistance = 110
insertionDistance = 80
frameCount = 0;

cylinderPosition1.x = 30
cylinderPosition1.y = -10
cylinderPosition1.z = -120
radius1 = 30

cylinderPosition2.x = -35
cylinderPosition2.y = 25
cylinderPosition2.z = -120
radius2 = 25

tension = 20
nPointsPerCylinderArc = 16

aviobj = avifile('two_cylinder_movie3d.avi','compression','None', 'fps', 25);
fig = figure(1);

count = 0;
for insertionAngle = 0: 20: 360
    for originAngle = 0: 20: 360
        originAngle2 = originAngle * pi / 180;
        insertionAngle2 = insertionAngle * pi / 180;
        
        origin.x = originDistance * cos(originAngle2)
        origin.y = originDistance * sin(originAngle2)
        origin.z = -50
        insertion.x = insertionDistance * cos(insertionAngle2)
        insertion.y = insertionDistance * sin(insertionAngle2)
        insertion.z = 80
        
        [wrapOK, originForce, insertionForce, cylinderForce1, cylinderForcePosition1, cylinderForce2, cylinderForcePosition2, pathLength, pathCoordinates] = ...
            TwoCylinderWrap(origin, insertion, cylinderPosition1, radius1, cylinderPosition2, radius2, tension, nPointsPerCylinderArc, pi());
        
        count = count + 1;
        pathLengths(count) = pathLength;
        
        figure(1);
        clf;
        hold on;
        
        draw_cylinder(cylinderPosition1, radius1, 240, 20, 'none');
        draw_cylinder(cylinderPosition2, radius2, 240, 20, 'none');
        
        line(pathCoordinates(:, 1), pathCoordinates(:, 2), pathCoordinates(:, 3), 'Color', 'black', 'LineWidth', 2);
        
        line([origin.x origin.x+originForce.x], [origin.y origin.y+originForce.y], [origin.z origin.z+originForce.z], 'Color', 'red', 'LineWidth', 4);
        line([insertion.x insertion.x+insertionForce.x], [insertion.y insertion.y+insertionForce.y], [insertion.z insertion.z+insertionForce.z], 'Color', 'blue', 'LineWidth', 4);
        line([cylinderForcePosition1.x cylinderForcePosition1.x+cylinderForce1.x], [cylinderForcePosition1.y cylinderForcePosition1.y+cylinderForce1.y], [cylinderForcePosition1.z cylinderForcePosition1.z+cylinderForce1.z], 'Color', 'green', 'LineWidth', 4);
        line([cylinderForcePosition2.x cylinderForcePosition2.x+cylinderForce2.x], [cylinderForcePosition2.y cylinderForcePosition2.y+cylinderForce2.y], [cylinderForcePosition2.z cylinderForcePosition2.z+cylinderForce2.z], 'Color', 'green', 'LineWidth', 4);
        
        axis equal;
        axis([-120,120,-120,120,-120,120]);
        xlabel('X');
        ylabel('Y');
        zlabel('Z');
        view(-37.5, 30); % default 3D
        
        F = getframe(fig);
        aviobj = addframe(aviobj, F);
        
        figure(2);
        plot(pathLengths);
        ylabel('Path Length (m)');
        
    end
end

aviobj = close(aviobj);

figure(2)
set(gcf,'PaperType','<custom>');
set(gcf,'Units','inches');
set(gcf,'PaperUnits','inches');
position = get(gcf,'Position');
set(gcf,'PaperPosition',[0,0,position(3:4)]);
set(gcf,'PaperSize',position(3:4));

print(gcf,'-dpdf','pathlength_plot.pdf','-r 150')
% print(gcf,'-depsc2','pathlength_plot.eps','-r 150')

return

function draw_circle(centre, radius, segments, colour)

x = zeros(segments + 1, 1);
y = zeros(segments + 1, 1);
for i = 1: segments    
    angle = (i / segments) * 2 * pi();
    x(i) = radius * cos(angle) + centre.x;
    y(i) = radius * sin(angle) + centre.y;
end
x(segments + 1) = x(1);
y(segments + 1) = y(1);
line(x, y, 'Color', colour);

return

function draw_cylinder(centre, radius, height, segments, colour)

[x, y, z] = cylinder(radius, segments);
surf(centre.x + x, centre.y + y, centre.z + height * z, 'FaceColor', colour);

return
    
    
