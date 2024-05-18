Feature: Tests the UserPort20To32Bit1 hardware with expected output
  
  
  Scenario: Execute schematic with test

    Then I create file "ModelConfig_VSMDD1.txt" with
      """
      PATTERN=TestData_UserPort20To32Bit1.txt
      EXITPROCCESSAFTER=0.50
      DELETETHISFILE=1
      """

    Given starting an automation process "cmd" with parameters: /c UserPort20To32Bit1.pdsprj
    When automation find window from pattern ".*UserPort20To32Bit1.*Proteus.*"
    When automation focus window
    When automation expand main menu item "Debug"
    When automation click current menu item "Run Simulation.*F12"
    Then automation wait for window close



  Scenario: RAM validation test 1

    Given open file "output\DebugUserPort20To32Bit1_RAMWrite.txt" for reading
    When ignoring lines that contain ";"
    Then expect the next line to contain "d$00013723"
    Then expect the next line to contain "d$00013834"
    Then expect the next line to contain "d$00013917"
    Then expect the next line to contain "d$00013a98"
    Then expect the next line to contain "d$00025311"
    Then expect the next line to contain "d$00025422"
    Then expect the next line to contain "d$00025533"
    Then expect the next line to contain "d$00025644"
