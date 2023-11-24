----------------------------------------------------------------------------------
-- Company: Agnikul Cosmos
-- Engineer: Anubhab
-- 
-- Create Date: 11/22/2023 08:38:27 PM
-- Design Name: 
-- Module Name: pre_processor_2 - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------
--declaration of my own package for integer array
library work;
package my_package is
    type integer_array is array (natural range <>) of integer;
end package my_package;

use work.my_package.all; 

--standard package
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;


-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity pre_processor_2 is
generic(block_size: integer := 4;
        max_limit : integer := 65000;
        min_limit : integer := 0);
    Port ( clock : in STD_LOGIC;
           reset : in STD_LOGIC;
           data_in : in integer_array(0 to block_size) := (others => 0);
           delta : inout integer_array(0 to block_size) := (others => 0);
           data_out : out integer_array(0 to block_size) := (others => 0));
end pre_processor_2;

architecture Behavioral of pre_processor_2 is

--    function to get the minimum of two int    
    function minimum_integer(x, y:integer) return integer is  
    begin
        if x<=y then
            return x;
        else
            return y;
        end if;
    end function minimum_integer;
    
--    fu7nction to calculate the delata element
    function delta_element(d,a,b,T : integer) return integer is
        variable delta : integer;
    begin
        if (0<=d and d<=T) then
            delta := 2*d;
        elsif (-T<=d and d<0) then
            delta := 2* (abs(d)) - 1;
        else
            delta := T + abs(d);
        end if;     
        return delta;
    end function delta_element;



begin   

    pre_processor:process(clock, reset)
    
--    variables declartion
    variable seq_delayed : integer_array(0 to block_size);
    variable d : integer_array(0 to block_size);
    variable delta_temp: integer_array(0 to block_size);
    variable a,b : integer :=0;
    variable T : integer :=0;
    
    begin
        if reset='1' then
            delta<=(others=>0);
        elsif rising_edge(clock)then
        
--            one unit delayed sequence
            seq_delayed := 0 & data_in(0 to block_size-1);
            
--            loop to calculate the intermidiate difference array d
            for i in 0 to block_size loop
                d(i) := data_in(i) - seq_delayed(i);
            end loop;     
            
--            calculate delta elements
            for i in 0 to block_size loop
                a := max_limit - seq_delayed(i);
                b := seq_delayed(i) - min_limit;
                T := minimum_integer(a,b);
                delta_temp(i) := delta_element(d(i),a,b,T);
            end loop;
        end if;    
--        assigning the delta array
        delta<=delta_temp;
    end process;
    

end Behavioral;
