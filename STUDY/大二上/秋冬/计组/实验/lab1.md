```
module seq(
	clk,
	reset,
	in,
	out
);
	input clk;
	input reset;
	input in;
	
	output out;
	parameter [2:0] s0=3'b000,
					s1=3'b001,
					s2=3'b010,
					s3=3'b011,
					s4=3'b100,
					s5=3'b101,
					s6=3'b110,
					s7=3'b111;
	reg [2:0] curr_state;
	reg [2:0] next_state;
	wire out;
	
	always @(posedge clk or negedge reset)
		begin
			if(!reset)
				curr_state<=s0;
			else
				curr_state<=next_state;
		end
	always @(curr_state or in)
		begin
			case(curr_state)
				s0:begin
					 if(in==0) next_state=s0;
					 else      next_state=s1;
					end
				s1:begin
					 if(in==0) next_state=s0;
					 else      next_state=s2;
					end
				s2:begin
					 if(in==0) next_state=s0;
					 else      next_state=s3;
					end
				s3:begin
					 if(in==0) next_state=s4;
					 else      next_state=s3;
					end
				s4:begin
					 if(in==0) next_state=s5;
					 else      next_state=s1;
					end
				s5:begin
					 if(in==0) next_state=s0;
					 else      next_state=s6;
					end
				s6:begin
					 if(in==0) next_state=s7;
					 else      next_state=s2;
					end
				s7:begin
					 if(in==0) next_state=s0;
					 else      next_state=s1;
					end 
				default:       next_state=s0;
			endcase
		end
		
		assign out=(curr_state==s7)?1:0;
endmodule
```