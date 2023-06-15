% Routine to analyse the Qualisys output and save kinematic match details from
% frames of interest

function orang()

page_screen_output = 0 % paging control

TorsoAngle=-1*2.8987402
RightThighAngle=-1*1.1837769
RightLegAngle=-1*1.1756871
RightFootAngle=-1*-0.9209639
LeftThighAngle=-1*1.7731668
LeftLegAngle=-1*1.7617466
LeftFootAngle=-1*-0.3343311
RightArmAngle=-1*2.0328418
RightForearmAngle=-1*1.9518007
RightHandAngle=-1*0.2407392 - 0.15 % -1*0.2407392 (edited to keep joint angle within normal range)
LeftArmAngle=-1*1.9408012
LeftForearmAngle=-1*1.1928667
LeftHandAngle=-1*-0.1626415

TorsoLength=0.600
ArmLength=0.360
ForearmLength=0.405
HandLength=0.250
ThighLength=0.300
LegLength=0.315
FootLength=0.250

TorsoCMOffset=0.189
ArmCMOffset=0.208
ForearmCMOffset=0.185
HandCMOffset=0.091
ThighCMOffset=0.123
LegCMOffset=0.138
FootCMOffset=0.110

LeftHipAngle=LeftThighAngle-TorsoAngle
LeftKneeAngle=LeftLegAngle-LeftThighAngle
LeftAnkleAngle=LeftFootAngle-LeftLegAngle
LeftShoulderAngle=LeftArmAngle-TorsoAngle
LeftElbowAngle=LeftForearmAngle-LeftArmAngle
LeftWristAngle=LeftHandAngle-LeftForearmAngle

RightHipAngle=RightThighAngle-TorsoAngle
RightKneeAngle=RightLegAngle-RightThighAngle
RightAnkleAngle=RightFootAngle-RightLegAngle
RightShoulderAngle=RightArmAngle-TorsoAngle
RightElbowAngle=RightForearmAngle-RightArmAngle
RightWristAngle=RightHandAngle-RightForearmAngle

LeftShoulder = [0, 0.2, 0.81]
TorsoCM = [LeftShoulder(1) + TorsoCMOffset * cos(TorsoAngle), 0, LeftShoulder(3) + TorsoCMOffset * sin(TorsoAngle)]

LeftElbow = [LeftShoulder(1) + ArmLength * cos(LeftArmAngle), LeftShoulder(2), LeftShoulder(3) + ArmLength * sin(LeftArmAngle)]
LeftWrist = [LeftElbow(1) + ForearmLength * cos(LeftForearmAngle), LeftElbow(2), LeftElbow(3) + ForearmLength * sin(LeftForearmAngle)]
LeftMC3 = [LeftWrist(1) + HandLength * cos(LeftHandAngle), LeftWrist(2), LeftWrist(3) + HandLength * sin(LeftHandAngle)]
LeftHip = [LeftShoulder(1) + TorsoLength * cos(TorsoAngle), 0.1, LeftShoulder(3) + TorsoLength * sin(TorsoAngle)]
LeftKnee = [LeftHip(1) + ThighLength * cos(LeftThighAngle), LeftHip(2), LeftHip(3) + ThighLength * sin(LeftThighAngle)]
LeftAnkle = [LeftKnee(1) + LegLength * cos(LeftLegAngle), LeftKnee(2), LeftKnee(3) + LegLength * sin(LeftLegAngle)]
LeftMT3 = [LeftAnkle(1) + FootLength * cos(LeftFootAngle), LeftAnkle(2), LeftAnkle(3) + FootLength * sin(LeftFootAngle)]

LeftArmCM = [LeftShoulder(1) + ArmCMOffset * cos(LeftArmAngle), LeftShoulder(2), LeftShoulder(3) + ArmCMOffset * sin(LeftArmAngle)]
LeftForearmCM = [LeftElbow(1) + ForearmCMOffset * cos(LeftForearmAngle), LeftElbow(2), LeftElbow(3) + ForearmCMOffset * sin(LeftForearmAngle)]
LeftHandCM = [LeftWrist(1) + HandCMOffset * cos(LeftHandAngle), LeftWrist(2), LeftWrist(3) + HandCMOffset * sin(LeftHandAngle)]
LeftThighCM = [LeftHip(1) + ThighCMOffset * cos(LeftThighAngle), LeftHip(2), LeftHip(3) + ThighCMOffset * sin(LeftThighAngle)]
LeftLegCM = [LeftKnee(1) + LegCMOffset * cos(LeftLegAngle), LeftKnee(2), LeftKnee(3) + LegCMOffset * sin(LeftLegAngle)]
LeftFootCM = [LeftAnkle(1) + FootCMOffset * cos(LeftFootAngle), LeftAnkle(2), LeftAnkle(3) + FootCMOffset * sin(LeftFootAngle)]

RightShoulder = [0, -0.2, 0.81]

RightElbow = [RightShoulder(1) + ArmLength * cos(RightArmAngle), RightShoulder(2), RightShoulder(3) + ArmLength * sin(RightArmAngle)]
RightWrist = [RightElbow(1) + ForearmLength * cos(RightForearmAngle), RightElbow(2), RightElbow(3) + ForearmLength * sin(RightForearmAngle)]
RightMC3 = [RightWrist(1) + HandLength * cos(RightHandAngle), RightWrist(2), RightWrist(3) + HandLength * sin(RightHandAngle)]
RightHip = [RightShoulder(1) + TorsoLength * cos(TorsoAngle), -0.1, RightShoulder(3) + TorsoLength * sin(TorsoAngle)]
RightKnee = [RightHip(1) + ThighLength * cos(RightThighAngle), RightHip(2), RightHip(3) + ThighLength * sin(RightThighAngle)]
RightAnkle = [RightKnee(1) + LegLength * cos(RightLegAngle), RightKnee(2), RightKnee(3) + LegLength * sin(RightLegAngle)]
RightMT3 = [RightAnkle(1) + FootLength * cos(RightFootAngle), RightAnkle(2), RightAnkle(3) + FootLength * sin(RightFootAngle)]

RightArmCM = [RightShoulder(1) + ArmCMOffset * cos(RightArmAngle), RightShoulder(2), RightShoulder(3) + ArmCMOffset * sin(RightArmAngle)]
RightForearmCM = [RightElbow(1) + ForearmCMOffset * cos(RightForearmAngle), RightElbow(2), RightElbow(3) + ForearmCMOffset * sin(RightForearmAngle)]
RightHandCM = [RightWrist(1) + HandCMOffset * cos(RightHandAngle), RightWrist(2), RightWrist(3) + HandCMOffset * sin(RightHandAngle)]
RightThighCM = [RightHip(1) + ThighCMOffset * cos(RightThighAngle), RightHip(2), RightHip(3) + ThighCMOffset * sin(RightThighAngle)]
RightLegCM = [RightKnee(1) + LegCMOffset * cos(RightLegAngle), RightKnee(2), RightKnee(3) + LegCMOffset * sin(RightLegAngle)]
RightFootCM = [RightAnkle(1) + FootCMOffset * cos(RightFootAngle), RightAnkle(2), RightAnkle(3) + FootCMOffset * sin(RightFootAngle)]

TorsoQuaternion = QFromAxisAngle(0, -1, 0, TorsoAngle)
LeftThighQuaternion = QFromAxisAngle(0, -1, 0, LeftThighAngle)
LeftLegQuaternion = QFromAxisAngle(0, -1, 0, LeftLegAngle)
LeftFootQuaternion = QFromAxisAngle(0, -1, 0, LeftFootAngle)
LeftArmQuaternion = QFromAxisAngle(0, -1, 0, LeftArmAngle)
LeftForearmQuaternion = QFromAxisAngle(0, -1, 0, LeftForearmAngle)
LeftHandQuaternion = QFromAxisAngle(0, -1, 0, LeftHandAngle)

RightThighQuaternion = QFromAxisAngle(0, -1, 0, RightThighAngle)
RightLegQuaternion = QFromAxisAngle(0, -1, 0, RightLegAngle)
RightFootQuaternion = QFromAxisAngle(0, -1, 0, RightFootAngle)
RightArmQuaternion = QFromAxisAngle(0, -1, 0, RightArmAngle)
RightForearmQuaternion = QFromAxisAngle(0, -1, 0, RightForearmAngle)
RightHandQuaternion = QFromAxisAngle(0, -1, 0, RightHandAngle)

% plot the start position
figure(1);
clf;
LeftSideJoint = [LeftMC3; LeftWrist; LeftElbow; LeftShoulder; LeftHip; LeftKnee; LeftAnkle; LeftMT3];
LeftSideCM = [LeftHandCM; LeftForearmCM; LeftArmCM; TorsoCM; LeftThighCM; LeftLegCM; LeftFootCM];
RightSideJoint = [RightMC3; RightWrist; RightElbow; RightShoulder; RightHip; RightKnee; RightAnkle; RightMT3];
RightSideCM = [RightHandCM; RightForearmCM; RightArmCM; TorsoCM; RightThighCM; RightLegCM; RightFootCM];

plot(LeftSideJoint(:, 1), LeftSideJoint(:, 3), 'r-', ...
    RightSideJoint(:, 1), RightSideJoint(:, 3), 'g-', ...
    LeftSideCM(:, 1), LeftSideCM(:, 3), 'ro', ...
    RightSideCM(:, 1), RightSideCM(:, 3), 'ro' ...
    );
axis('equal');
axis('auto');
title('Start Position');
legend('Left','Right', 'LeftCM', 'RightCM');

figure(2);
clf;
LeftSideJoint = [LeftMC3; LeftWrist; LeftElbow; LeftShoulder; LeftHip; LeftKnee; LeftAnkle; LeftMT3];
LeftSideCM = [LeftHandCM; LeftForearmCM; LeftArmCM; TorsoCM; LeftThighCM; LeftLegCM; LeftFootCM];
RightSideJoint = [RightMC3; RightWrist; RightElbow; RightShoulder; RightHip; RightKnee; RightAnkle; RightMT3];
RightSideCM = [RightHandCM; RightForearmCM; RightArmCM; TorsoCM; RightThighCM; RightLegCM; RightFootCM];

plot(LeftSideJoint(:, 1), LeftSideJoint(:, 2), 'r-', ...
    RightSideJoint(:, 1), RightSideJoint(:, 2), 'g-', ...
    LeftSideCM(:, 1), LeftSideCM(:, 2), 'ro', ...
    RightSideCM(:, 1), RightSideCM(:, 2), 'ro' ...
    );
axis('equal');
axis('auto');
title('Start Position');
legend('Left','Right', 'LeftCM', 'RightCM');

fout = fopen('orang.txt', 'w');

fprintf(fout, 'TorsoCM \"%.17f %.17f %.17f\"\n', ...
    TorsoCM(1), TorsoCM(2), TorsoCM(3));
fprintf(fout, 'TorsoQuaternion \"%.17f %.17f %.17f %.17f\"\n', ...
    TorsoQuaternion.n, TorsoQuaternion.x, TorsoQuaternion.y, TorsoQuaternion.z);

fprintf(fout, 'LeftThighCM \"%.17f %.17f %.17f\"\n', ...
    LeftThighCM(1), LeftThighCM(2), LeftThighCM(3));
fprintf(fout, 'LeftThighQuaternion \"%.17f %.17f %.17f %.17f\"\n', ...
    LeftThighQuaternion.n, LeftThighQuaternion.x, LeftThighQuaternion.y, LeftThighQuaternion.z);

fprintf(fout, 'LeftLegCM \"%.17f %.17f %.17f\"\n', ...
    LeftLegCM(1), LeftLegCM(2), LeftLegCM(3));
fprintf(fout, 'LeftLegQuaternion \"%.17f %.17f %.17f %.17f\"\n', ...
    LeftLegQuaternion.n, LeftLegQuaternion.x, LeftLegQuaternion.y, LeftLegQuaternion.z);

fprintf(fout, 'LeftFootCM \"%.17f %.17f %.17f\"\n', ...
    LeftFootCM(1), LeftFootCM(2), LeftFootCM(3));
fprintf(fout, 'LeftFootQuaternion \"%.17f %.17f %.17f %.17f\"\n', ...
    LeftFootQuaternion.n, LeftFootQuaternion.x, LeftFootQuaternion.y, LeftFootQuaternion.z);

fprintf(fout, 'LeftArmCM \"%.17f %.17f %.17f\"\n', ...
    LeftArmCM(1), LeftArmCM(2), LeftArmCM(3));
fprintf(fout, 'LeftArmQuaternion \"%.17f %.17f %.17f %.17f\"\n', ...
    LeftArmQuaternion.n, LeftArmQuaternion.x, LeftArmQuaternion.y, LeftArmQuaternion.z);

fprintf(fout, 'LeftForearmCM \"%.17f %.17f %.17f\"\n', ...
    LeftForearmCM(1), LeftForearmCM(2), LeftForearmCM(3));
fprintf(fout, 'LeftForearmQuaternion \"%.17f %.17f %.17f %.17f\"\n', ...
    LeftForearmQuaternion.n, LeftForearmQuaternion.x, LeftForearmQuaternion.y, LeftForearmQuaternion.z);

fprintf(fout, 'LeftHandCM \"%.17f %.17f %.17f\"\n', ...
    LeftHandCM(1), LeftHandCM(2), LeftHandCM(3));
fprintf(fout, 'LeftHandQuaternion \"%.17f %.17f %.17f %.17f\"\n', ...
    LeftHandQuaternion.n, LeftHandQuaternion.x, LeftHandQuaternion.y, LeftHandQuaternion.z);

fprintf(fout, 'RightThighCM \"%.17f %.17f %.17f\"\n', ...
    RightThighCM(1), RightThighCM(2), RightThighCM(3));
fprintf(fout, 'RightThighQuaternion \"%.17f %.17f %.17f %.17f\"\n', ...
    RightThighQuaternion.n, RightThighQuaternion.x, RightThighQuaternion.y, RightThighQuaternion.z);

fprintf(fout, 'RightLegCM \"%.17f %.17f %.17f\"\n', ...
    RightLegCM(1), RightLegCM(2), RightLegCM(3));
fprintf(fout, 'RightLegQuaternion \"%.17f %.17f %.17f %.17f\"\n', ...
    RightLegQuaternion.n, RightLegQuaternion.x, RightLegQuaternion.y, RightLegQuaternion.z);

fprintf(fout, 'RightFootCM \"%.17f %.17f %.17f\"\n', ...
    RightFootCM(1), RightFootCM(2), RightFootCM(3));
fprintf(fout, 'RightFootQuaternion \"%.17f %.17f %.17f %.17f\"\n', ...
    RightFootQuaternion.n, RightFootQuaternion.x, RightFootQuaternion.y, RightFootQuaternion.z);

fprintf(fout, 'RightArmCM \"%.17f %.17f %.17f\"\n', ...
    RightArmCM(1), RightArmCM(2), RightArmCM(3));
fprintf(fout, 'RightArmQuaternion \"%.17f %.17f %.17f %.17f\"\n', ...
    RightArmQuaternion.n, RightArmQuaternion.x, RightArmQuaternion.y, RightArmQuaternion.z);

fprintf(fout, 'RightForearmCM \"%.17f %.17f %.17f\"\n', ...
    RightForearmCM(1), RightForearmCM(2), RightForearmCM(3));
fprintf(fout, 'RightForearmQuaternion \"%.17f %.17f %.17f %.17f\"\n', ...
    RightForearmQuaternion.n, RightForearmQuaternion.x, RightForearmQuaternion.y, RightForearmQuaternion.z);

fprintf(fout, 'RightHandCM \"%.17f %.17f %.17f\"\n', ...
    RightHandCM(1), RightHandCM(2), RightHandCM(3));
fprintf(fout, 'RightHandQuaternion \"%.17f %.17f %.17f %.17f\"\n', ...
    RightHandQuaternion.n, RightHandQuaternion.x, RightHandQuaternion.y, RightHandQuaternion.z);

fprintf(fout, 'LeftHip \"%.17f %.17f %.17f\"\n', LeftHip);
fprintf(fout, 'LeftKnee \"%.17f %.17f %.17f\"\n', LeftKnee);
fprintf(fout, 'LeftAnkle \"%.17f %.17f %.17f\"\n', LeftAnkle);
fprintf(fout, 'LeftMT3 \"%.17f %.17f %.17f\"\n', LeftMT3);
fprintf(fout, 'LeftShoulder \"%.17f %.17f %.17f\"\n', LeftShoulder);
fprintf(fout, 'LeftElbow \"%.17f %.17f %.17f\"\n', LeftElbow);
fprintf(fout, 'LeftWrist \"%.17f %.17f %.17f\"\n', LeftWrist);
fprintf(fout, 'LeftMC3 \"%.17f %.17f %.17f\"\n', LeftMC3);

fprintf(fout, 'RightHip \"%.17f %.17f %.17f\"\n', RightHip);
fprintf(fout, 'RightKnee \"%.17f %.17f %.17f\"\n', RightKnee);
fprintf(fout, 'RightAnkle \"%.17f %.17f %.17f\"\n', RightAnkle);
fprintf(fout, 'RightMT3 \"%.17f %.17f %.17f\"\n', RightMT3);
fprintf(fout, 'RightShoulder \"%.17f %.17f %.17f\"\n', RightShoulder);
fprintf(fout, 'RightElbow \"%.17f %.17f %.17f\"\n', RightElbow);
fprintf(fout, 'RightWrist \"%.17f %.17f %.17f\"\n', RightWrist);
fprintf(fout, 'RightMC3 \"%.17f %.17f %.17f\"\n', RightMC3);

fprintf(fout, 'LeftHipAngle \"%.17f\"\n', LeftHipAngle);
fprintf(fout, 'LeftKneeAngle \"%.17f\"\n', LeftKneeAngle);
fprintf(fout, 'LeftAnkleAngle \"%.17f\"\n', LeftAnkleAngle);
fprintf(fout, 'LeftShoulderAngle \"%.17f\"\n', LeftShoulderAngle);
fprintf(fout, 'LeftElbowAngle \"%.17f\"\n', LeftElbowAngle);
fprintf(fout, 'LeftWristAngle \"%.17f\"\n', LeftWristAngle);

fprintf(fout, 'RightHipAngle \"%.17f\"\n', RightHipAngle);
fprintf(fout, 'RightKneeAngle \"%.17f\"\n', RightKneeAngle);
fprintf(fout, 'RightAnkleAngle \"%.17f\"\n', RightAnkleAngle);
fprintf(fout, 'RightShoulderAngle \"%.17f\"\n', RightShoulderAngle);
fprintf(fout, 'RightElbowAngle \"%.17f\"\n', RightElbowAngle);
fprintf(fout, 'RightWristAngle \"%.17f\"\n', RightWristAngle);

fclose(fout);

% calculate quaternion from an axis and an angle
% [x, y, z] must be normalised (length 1) before calling this routine
function q = QFromAxisAngle(x, y, z, angle)

sin_a = sin( angle / 2 );
cos_a = cos( angle / 2 );

q.x    = x * sin_a;
q.y    = y * sin_a;
q.z    = z * sin_a;
q.n    = cos_a;

return;

