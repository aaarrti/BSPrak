ENTRY(_start)
SECTIONS
{
    . = 0x00100000;
    .kernel_code : {
                _kernel_code_start = .;
                *(.init)
                *libe3.a:*(.init)
                *libe3.a:*(.text .text.*)
                *libe3.a:*(.rodata .rodata.*)
                driver/?*(.text*)
                driver/?*(.rodata*)
                lib/?*(.text*)
                lib/?*(.rodata*)
                system/?*(.text*)
                system/?*(.rodata*)
                _kernel_code_end = .;
        }



    . = 0x00200000;
    .kernel_data : {
                _kernel_data_start = .;
                *libe3.a:*(.data .data.*)
                *libe3.a:*(.bss .bss.*)
                driver/?*(.data*)
                driver/?*(.bss*)
                lib/?*(.data*)
                lib/?*(.bss*)
                system/?*(.data*)
                system/?*(.bss*)
                _kernel_data_end = .;
         }
    .user_data : {
            _user_data_start = .;
            user/?*(.data*)
            user/?*(.bss*)
            _user_data_end = .;
    }




    . = 0x00500000;
    .user_code : {
                _user_code_start = .;
                user/?*(.text*)
                user/?*(.rodata*)
                _user_code_end = .;
        }



    . = 0x00600000;
    .user_data : {
                _user_data_start = .;
                user/?*(.data*)
                user/?*(.bss*)
                _user_data_end = .;
        }


}

