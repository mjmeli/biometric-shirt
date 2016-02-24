delete(instrfind)
clear s;

s = serial('/dev/tty.usbserial-DN00CXW0');
s.ReadAsyncMode = 'continuous';
s.BaudRate = 9600;
    
fopen(s);
while(s.Status == 'open')
    out = fscanf(s);
    fprintf('%c', out);
end