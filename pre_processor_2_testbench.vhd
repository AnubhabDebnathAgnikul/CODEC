----------------------------------------------------------------------------------
-- Company: Agnikul Cosmos
-- Engineer: Anubhab
-- 
-- Create Date: 11/22/2023 08:46:27 PM
-- Design Name: 
-- Module Name: pre_processor_2_testbench - Behavioral
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
library work;
package my_package is
    type integer_array is array (natural range <>) of integer;
end package my_package;

use work.my_package.all; 

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
use STD.textio.all;
use IEEE.std_logic_textio.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity pre_processor_2_testbench is
--  Port ( );
generic(block_size: integer := 4;
        max_limit : integer := 65000;
        min_limit : integer := 0);
end pre_processor_2_testbench;

architecture Behavioral of pre_processor_2_testbench is
    signal clock, reset : std_logic := '0';
    signal data_in   : integer_array(0 to block_size) := (105,243,0,42,56);
    signal delta : integer_array(0 to block_size) := (others => 0);
    signal data_out  : integer_array(0 to block_size) ;

        -- variables
    file input_file     :   TEXT open READ_MODE is "/home/level1/Desktop/Anubhab_Codespace/CODEC/data_file.txt";
    file output_file    :   TEXT open WRITE_MODE is "/home/level1/Desktop/Anubhab_Codespace/CODEC/output.txt";
    
component pre_processor_2
    port(
    clock : std_logic;
    reset   : std_logic;
    data_in : in integer_array(0 to block_size);
    delta : inout integer_array(0 to block_size) := (others => 0);
    data_out : out integer_array(0 to block_size)
    );
end component;

begin

    uut: pre_processor_2
        port map(clock => clock, reset => reset, data_in => data_in, data_out => data_out, delta=>delta);
        
        -- clokc process
        process
        begin
            clock <= '0';
            wait for 1 ms;
            clock <= '1';
            wait for 1 ms;
        end process;
    
    
        --stimulus process
        process
         --line buffer
        variable input_line : LINE;
        variable output_line: LINE;
        
        -- variable to store the line
        variable line_register : integer;
        begin
            -- apply reset
            reset <= '1';
          
            for i in 0 to block_size loop
                --read a line
                readline(input_file, input_line);
                --store
                read(input_line, line_register);                    
                --input to data_in
                data_in(i)<= line_register;                                        
            end loop;
  
            wait for 10 ns;
            reset <= '0';
            wait until rising_edge(clock);
            wait for 10 ns;
            for i in 0 to block_size loop
                write(output_line, delta(i));
                writeline(output_file,output_line);
            end loop;
            wait;
        end process; 

end Behavioral;
