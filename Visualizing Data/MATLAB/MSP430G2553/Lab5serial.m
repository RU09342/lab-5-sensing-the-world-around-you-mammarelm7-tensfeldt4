%Receive Serial Sensor MSP430G2553
%Gets 3 sensor reads over 3 seconds
%Matt Mammarelli
%9/27/17


delete(instrfindall); %prevents cant use com port error

clear all;
clf
Ts = 1;   % Sampling time
t = 1:Ts:6; % support of signal

p=serial('COM8','BaudRate',9600,'DataBits',8); %have to change com to current one
set(p,'Parity','none');
set(p,'Terminator','LF');
fopen(p); %opens com port
fprintf(p,'A') %sends character A to UART to receive sensor info


pause(1); %delay one second
fprintf(p,'A')


pause(1); %delay one second
fprintf(p,'A')


veri=fscanf(p,'%u'); %gets UART sensor information

figure %creates figure
xlabel('Count')
ylabel('Sense Change')
title('Photoresistor Count vs. Sense')
grid on;
hold on;


for t=1:Ts:6
    if (mod(t,2)==1) %only use odd matrix indices, evens are return characters
    plot(t,uint8(veri(t)),'o'); %plots circles 
    end
    xlim ([0 6]) %x limit
    ylim ([0 200]) %y limit
    drawnow    
end

    

fclose(p); %closes serial connection
delete(p); %deletes serial variable




