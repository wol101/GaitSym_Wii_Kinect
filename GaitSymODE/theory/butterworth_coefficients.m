function butterworth_coefficients()
% this function outputs butterworth coefficients for a set of useful
% lowpass filters that can be used in IIR implentations

% the output format is:

% frequency a[1] a[2] ... a[n] b[1] b[2] ... b[n]
% frequency is the divisor of the sampling frequency

out = fopen('Butterworth Coefficients.txt', 'w')

order = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
frequency = [5 10 20 50 100 200 500 1000 2000 5000 10000]

for o = 1: length(order)
    fprintf(out, 'order %d\n', order(o));
    
    fprintf(out, 'fcutoff');
    for j = 1: order(o) + 1
        fprintf(out, ' b[%d]', j - 1);
    end
    for j = 1: order(o) + 1
        fprintf(out, ' a[%d]', j - 1);
    end
    fprintf(out, '\n');
    
    for f = 1: length(frequency)
        [b, a] = butter(order(o), 2 / frequency(f), 'low');
        fprintf(out, '%.17g', frequency(f));
        for j = 1: length(a)
            fprintf(out, ' %.17g', b(j));
        end
        for j = 1: length(b)
            fprintf(out, ' %.17g', a(j));
        end
        fprintf(out, '\n');
    end
    
end

fprintf(out, '\n');
for o = 1: length(order)
    fprintf(out, 'order %d\n', order(o));
    
    fprintf(out, 'h[z] = (b[0]');
    for j = 1: order(o)
        fprintf(out, '+b[%d]*h[z-%d]', j, j);
    end
    fprintf(out, ')/((a[0]');
    for j = 1: order(o)
        fprintf(out, '+a[%d]*h[z-%d]', j, j);
    end
    fprintf(out, ');\n');
        
    for f = 1: length(frequency)
        [b, a] = butter(order(o), 2 / frequency(f), 'low');
        fprintf(out, 'double f=%.17g;\n', frequency(f));
        fprintf(out, 'double b[]={%.17g',b(1));
        for j = 2: length(a)
            fprintf(out, ',%.17g', b(j));
        end
        fprintf(out, '};\n');
        
        fprintf(out, 'double a[]={%.17g',a(1));
        for j = 1: length(b)
            fprintf(out, ',%.17g', a(j));
        end
        fprintf(out, '};\n\n');
    end
    
end


fclose(out);

