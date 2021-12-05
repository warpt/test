clear all 
close all 
 
obj = serial('COM15','BaudRate',115200); 
fopen(obj) 
file = fopen('Time_Capsule.txt','r'); 
%z=fread(obj,512,'uint8') 
a = uint8(fscanf(file,'%c')); 
response = uint8(zeros(1,length(a))); 
for i=1:length(a) 
    fwrite(obj,a(i),'uint8'); 
    response(i) = fread(obj,1,'uint8'); 
    if (a(i) ~= response(i))  
  warning("Send and received byte not equal");  
     end 
end 
fwrite(obj,26,'uint8') 
if (fread(obj,1,'uint8') ~= 26) 
 warning("Not Synchro")
end
b(1) = fread(obj,1,'uint8') 
i=2;
while (b(i-1)~=26)
    b(i) = fread(obj,1,'uint8');
    i=i+1;
end
char(b)
status=fread(obj,1,'uint8')
fclose(file);
fclose(obj);