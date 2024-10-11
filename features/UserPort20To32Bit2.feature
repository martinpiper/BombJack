Feature: Tests the UserPort20To32Bit2 hardware with expected output
  
  
  Scenario: Execute schematic with test

    Then I create file "ModelConfig_VSMDD1.txt" with
      """
      PATTERN=TestData_UserPort20To32Bit2.txt
      EXITPROCCESSAFTER=0.50
      DELETETHISFILE=1
      """

    Given starting an automation process "cmd" with parameters: /c UserPort20To32Bit2.pdsprj
    When automation find window from pattern ".*UserPort20To32Bit2.*Proteus.*"
    When automation focus window
    When automation expand main menu item "Debug"
    When automation click current menu item "Run Simulation.*F12"
    Then automation wait for window close



  Scenario: RAM validation test 1, for the full execution

    Given open file "output\DebugUserPort20To32Bit2_RAMWrite.txt" for reading
    When ignoring lines that contain ";"
    When ignoring empty lines
    Then expect the next line to contain "d$100207dc"
    Then expect the next line to contain "d$100208ed"
    Then expect the next line to contain "d$100209fe"
    Then expect the next line to contain "d$10020a0f"
    Then expect the next line to contain "d$00013723"
    Then expect the next line to contain "d$00013834"
    Then expect the next line to contain "d$00013917"
    Then expect the next line to contain "d$00013a98"
    Then expect the next line to contain "d$00025311"
    Then expect the next line to contain "d$00025422"
    Then expect the next line to contain "d$00025533"
    Then expect the next line to contain "d$00025644"
    # Signal bytes ready from FTDI
    Then expect the next line to contain "d$00032374"
    Then expect the next line to contain "d$0003248b"



  Scenario: Validate passthrough 1
    When processing each line in file "output\DebugUserPort20To32Bit2_PassthroughChange.txt" and only output to file "target/out.txt" lines after finding a line containing "During power-on reset"
    Given open file "target/out.txt" for reading
    When ignoring lines that contain ";"
    When ignoring lines that contain "."
    When ignoring empty lines
    Then accounting for transient values expect the next line to contain "d=PT_PC"
    Then accounting for transient values expect the next line to contain "d=PT_PC | $00000001"
    Then accounting for transient values expect the next line to contain "d=PT_PC | $00000002"
    Then accounting for transient values expect the next line to contain "d=PT_PC | $00000003"
    Then accounting for transient values expect the next line to contain "d=PT_PC | $00000004"



  Scenario: Validate passthrough 2
    When processing each line in file "output\DebugUserPort20To32Bit2_PassthroughChange.txt" and only output to file "target/out.txt" lines after finding a line containing "After power-on reset"
    Given open file "target/out.txt" for reading
    When ignoring lines that contain ";"
    When ignoring lines that contain "."
    When ignoring empty lines
    Then expect the next line to contain "d=PTPA2 | PT_PC | $00000004"
    Then expect the next line to contain "d=PTPA2 | PT_PC"
    Then expect the next line to contain "d=PTPA2 | PT_PC | $00000001"
    # Transient
    Then expect the next line to contain "d=PTPA2 | PT_PC | $00000003"
#    Then expect the next line to contain "d=PT_PC | $00000001"
    Then accounting for transient values expect the next line to contain "d=PT_PC | $00000002"
    Then expect the next line to contain "d=PT_PC | $00000003"
    Then expect the next line to contain "d=PTPA2 | PT_PC | $00000003"
    Then expect the next line to contain "d=PTPA2 | PT_PC"
    Then expect the next line to contain "d=PTPA2 | PT_PC | $00000001"
    Then expect the next line to contain "d=PTPA2 | $00000001"
    Then expect the next line to contain "d=PTPA2 | PT_PC | $00000001"
    Then accounting for transient values expect the next line to contain "d=PTPA2 | PT_PC | $00000002"
    Then expect the next line to contain "d=PTPA2 | $00000002"
    Then expect the next line to contain "d=PTPA2 | PT_PC | $00000002"
    Then expect the next line to contain "d=PTPA2 | PT_PC | $00000003"
    Then expect the next line to contain "d=PTPA2 | $00000003"
    Then expect the next line to contain "d=PTPA2 | PT_PC | $00000003"



  Scenario: Validate disabled mode 1
    When processing each line in file "output\DebugUserPort20To32Bit2_PassthroughChange.txt" and only output to file "target/out.txt" lines after finding a line containing "Set latch7 - Disabled mode"
    Given open file "target/out.txt" for reading
    When ignoring lines that contain ";"
    When ignoring lines that contain "."
    When ignoring empty lines
    Then expect the next line to contain "d=PTPA2 | PT_PC | $00000083"
    Then expect the next line to contain "d=PT_PC | $00000083"
    Then expect the next line to contain "d=PTPA2 | PT_PC | $00000083"
    Then expect the next line to contain "d=PTPA2 | PT_PC"



  Scenario: Validate passthrough 3
    When processing each line in file "output\DebugUserPort20To32Bit2_PassthroughChange.txt" and only output to file "target/out.txt" lines after finding a line containing "Set latch7 - Enable pass-through"
    Given open file "target/out.txt" for reading
    When ignoring lines that contain ";"
    When ignoring lines that contain "."
    When ignoring empty lines
    Then expect the next line to contain "d=PTPA2 | PT_PC | $00000080"
    Then expect the next line to contain "d=PT_PC | $00000080"



  Scenario: Validate PS2 toggle 1
    When processing each line in file "output\DebugUserPort20To32Bit2_PassthroughChange.txt" and only output to file "target/out.txt" lines after finding a line containing "PA2 toggle twice"
    Given open file "target/out.txt" for reading
    When ignoring lines that contain ";"
    When ignoring lines that contain "."
    When ignoring empty lines
    Then expect the next line to contain "d=PT_PC"
    Then expect the next line to contain "d=PTPA2 | PT_PC"
    Then expect the next line to contain "d=PTPA2 | PT_PC"
    Then expect the next line to contain "d=PT_PC"
    Then expect the next line to contain "d=PT_PC"
    Then expect the next line to contain "d=PTPA2 | PT_PC"
    Then expect the next line to contain "d=PTPA2 | PT_PC"
    Then expect the next line to contain "d=PT_PC"
    Then expect the next line to contain "d=PT_PC"
    Then expect the next line to contain "d=PTPA2 | PT_PC"



  Scenario: Validate direct bytes with latch4 1
    When processing each line in file "output\DebugUserPort20To32Bit2_PassthroughChange.txt" and only output to file "target/out.txt" lines after finding a line containing "Write some direct bytes to the pass-through with latch4"
    Given open file "target/out.txt" for reading
    When ignoring lines that contain ";"
    When ignoring lines that contain "."
    When ignoring empty lines
    Then accounting for transient values expect the next line to contain "d=PTPA2 | PT_PC | $00000055"
    Then accounting for transient values expect the next line to contain "d=PTPA2 | $00000055"
    Then accounting for transient values expect the next line to contain "d=PTPA2 | PT_PC | $00000055"
    Then accounting for transient values expect the next line to contain "d=PTPA2 | PT_PC | $00000066"
    Then accounting for transient values expect the next line to contain "d=PTPA2 | $00000066"
    Then accounting for transient values expect the next line to contain "d=PTPA2 | PT_PC | $00000066"
    Then accounting for transient values expect the next line to contain "d=PTPA2 | PT_PC | $00000077"
    Then accounting for transient values expect the next line to contain "d=PTPA2 | $00000077"
    Then accounting for transient values expect the next line to contain "d=PTPA2 | PT_PC | $00000077"
    Then accounting for transient values expect the next line to contain "d=PTPA2 | PT_PC | $00000088"
    Then accounting for transient values expect the next line to contain "d=PTPA2 | $00000088"
    Then accounting for transient values expect the next line to contain "d=PTPA2 | PT_PC | $00000088"



  Scenario: Validate RAM read to back to C64 1
    When processing each line in file "output\DebugUserPort20To32Bit2.txt" and only output to file "target/out.txt" lines after finding a line containing "Set latch5 again for read RAM"
    Given open file "target/out.txt" for reading
    When ignoring lines that contain ";"
    When ignoring lines that contain "."
    When ignoring empty lines
    Then expect the next line to contain "d$00013723"
    Then expect the next line to contain "d$00013834"
    Then expect the next line to contain "d$00013917"
    Then expect the next line to contain "d$00013a98"
    Then expect the next line to contain "d$00013b00"
    Then expect the next line to contain "d$00025311"
    Then expect the next line to contain "d$00025422"
    Then expect the next line to contain "d$00025533"
    Then expect the next line to contain "d$00025644"



  Scenario: Validate RAM read to pass-through 1
    When processing each line in file "output\DebugUserPort20To32Bit2_PassthroughChange.txt" and only output to file "target/out.txt" lines after finding a line containing "Set latch7 - Enable RAM to pass-through"
    Given open file "target/out.txt" for reading
    When ignoring lines that contain ";"
    When ignoring lines that contain "."
    When ignoring empty lines
    Then expect the next line to contain "d=PTPA2 | PT_PC"
    Then expect the next line to contain "d=PTPA2 | PT_PC"
    Then expect the next line to contain "d=PTPA2 | PT_PC"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000023"
    Then accounting for transient values expect the next line to contain "PTPA2 | $00000023"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000023"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000034"
    Then accounting for transient values expect the next line to contain "PTPA2 | $00000034"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000034"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000017"
    Then accounting for transient values expect the next line to contain "PTPA2 | $00000017"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000017"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000098"
    Then accounting for transient values expect the next line to contain "PTPA2 | $00000098"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000098"


  Scenario: Validate RAM read to pass-through 2
    When processing each line in file "output\DebugUserPort20To32Bit2_PassthroughChange.txt" and only output to file "target/out.txt" lines after finding a line containing "Read from latch3 which should read the added address..."
    Given open file "target/out.txt" for reading
    When ignoring lines that contain ";"
    When ignoring lines that contain "."
    When ignoring empty lines
    Then expect the next line to contain "d=PTPA2 | PT_PC"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000011"
    Then accounting for transient values expect the next line to contain "PTPA2 | $00000011"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000011"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000022"
    Then accounting for transient values expect the next line to contain "PTPA2 | $00000022"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000022"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000033"
    Then accounting for transient values expect the next line to contain "PTPA2 | $00000033"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000033"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000044"
    Then accounting for transient values expect the next line to contain "PTPA2 | $00000044"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000044"




  Scenario: Fast DMA test 1
    When processing each line in file "output\DebugUserPort20To32Bit2_PassthroughChange.txt" and only output to file "target/out.txt" lines after finding a line containing "Fast DMA test"
    Given open file "target/out.txt" for reading
    When ignoring lines that contain ";"
    When ignoring lines that contain "."
    When ignoring empty lines
    Then expect the next line to contain "d=PTPA2 | PT_PC"
    Then expect the next line to contain "d=PTPA2 | PT_PC"
    Then expect the next line to contain "d=PTPA2 | PT_PC"
    Then expect the next line to contain "d=PTPA2 | PT_PC"
    Then expect the next line to contain "d=PTPA2 | PT_PC"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000023"
    Then accounting for transient values expect the next line to contain "PTPA2 | $00000023"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000023"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000034"
    Then accounting for transient values expect the next line to contain "PTPA2 | $00000034"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000034"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000017"
    Then accounting for transient values expect the next line to contain "PTPA2 | $00000017"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000017"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000098"
    Then accounting for transient values expect the next line to contain "PTPA2 | $00000098"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000098"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC"
    Then accounting for transient values expect the next line to contain "PTPA2"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC"
    Given close current file


    When processing each line in file "output\DebugUserPort20To32Bit2_PassthroughChange.txt" and only output to file "target/out.txt" lines after finding a line containing "Around here should be more non-zero bytes read"
    Given open file "target/out.txt" for reading
    When ignoring lines that contain ";"
    When ignoring lines that contain "."
    When ignoring empty lines
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000011"
    Then accounting for transient values expect the next line to contain "PTPA2 | $00000011"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000011"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000022"
    Then accounting for transient values expect the next line to contain "PTPA2 | $00000022"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000022"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000033"
    Then accounting for transient values expect the next line to contain "PTPA2 | $00000033"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000033"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $0000004"
    Then accounting for transient values expect the next line to contain "PTPA2 | $00000044"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC | $00000044"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC"
    Then accounting for transient values expect the next line to contain "PTPA2"
    Then accounting for transient values expect the next line to contain "PTPA2 | PT_PC"
    Given close current file


    When processing each line in file "output\DebugUserPort20To32Bit2.txt" and only output to file "target/out.txt" lines after finding a line containing "DMA Still in progress"
    Given open file "target/out.txt" for reading
    When ignoring lines that contain ";"
    When ignoring lines that contain "."
    When ignoring empty lines
    Then expect the next line to contain "d$000320ff"
    Then expect the next line to contain "d$000323fe"
    Then expect the next line to contain "d$000323fe"
    Then expect the next line to contain "d$000323fe"
    Then expect the next line to contain "d$000323fe"
    Then expect the next line to contain "d$000323fe"
    Then expect the next line to contain "d$000323fe"

