module top(input a,input b, input c,input d, input e,input f, output z,output y);


assign y=a&b&c&d&e;
assign z=y|f;


endmodule
