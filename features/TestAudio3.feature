Feature: Tests Audio3 design

  Tests the Audio3 design by validating files output by the simulation.


  Scenario: Validates expected data from unit test input

#    Given I create file "ModelConfig_VSMDD2.txt" with
#    """
#    EXITPROCCESSAFTER=0.5
#    DELETETHISFILE=1
#    """

#    Given starting an automation process "cmd" with parameters: /c Audio3.pdsprj
#    Given wait for 5000 milliseconds
#    When automation find window from pattern ".*Audio3.*Proteus.*"
#    Given wait for 5000 milliseconds
#    When automation focus window
#    When automation wait for idle
#    When automation expand main menu item "Debug"
#    Given wait for 5000 milliseconds
#    When automation wait for idle
#    When automation click current menu item "Run Simulation"
#    Then automation wait for window close
    

    Given open file "output\DebugAudio3Output.txt" for reading
    When ignoring lines that contain ";"
    When ignoring empty lines
    Then expect the next line to contain "d$00000000"
    Then expect the next line to contain "d$00000080"
    Then expect the next line to contain "d$00000080"
    Then expect the next line to contain "d$00000080"
    Then expect the next line to contain "d$00000081"
    Then expect the next line to contain "d$00000081"
    Then expect the next line to contain "d$00000081"
    Then expect the next line to contain "d$00000081"
    Then expect the next line to contain "d$00000081"
    Then expect the next line to contain "d$00000081"
    Then expect the next line to contain "d$00000081"
    Then expect the next line to contain "d$00000081"
    Then expect the next line to contain "d$00000090"
    Then expect the next line to contain "d$00000090"
    Then expect the next line to contain "d$00000090"
    Then expect the next line to contain "d$00000090"
    Then expect the next line to contain "d$00000090"
    Then expect the next line to contain "d$00000090"
    Then expect the next line to contain "d$00000090"
    Then expect the next line to contain "d$00000090"
    Then expect the next line to contain "d$00000070"
    Then expect the next line to contain "d$00000070"
    Then expect the next line to contain "d$00000070"
    Then expect the next line to contain "d$00000070"
    Then expect the next line to contain "d$00000070"
    Then expect the next line to contain "d$00000070"
    Then expect the next line to contain "d$000000ef"
    Then expect the next line to contain "d$000000ef"
    Then expect the next line to contain "d$00000070"
    Then expect the next line to contain "d$00000070"
    Then expect the next line to contain "d$00000000"
    Then expect the next line to contain "d$00000000"
    Then expect the next line to contain "d$0000007f"
    Then expect the next line to contain "d$0000007f"
    Then expect the next line to contain "d$00000040"
    Then expect the next line to contain "d$00000040"
    Then expect the next line to contain "d$00000040"
    Then expect the next line to contain "d$00000040"
    Then expect the next line to contain "d$00000080"
    Then expect the next line to contain "d$00000080"
    Then expect the next line to contain "d$00000080"
    Then expect the next line to contain "d$00000080"
    Then expect the next line to contain "d$000000a0"
    Then expect the next line to contain "d$000000a0"
    Then expect the next line to contain "d$000000a0"
    Then expect the next line to contain "d$000000a0"
    Then expect the next line to contain "d$000000b0"
    Then expect the next line to contain "d$000000b0"
    Then expect the next line to contain "d$000000b0"
    Then expect the next line to contain "d$000000b0"
    Then expect the next line to contain "d$000000e0"
    Then expect the next line to contain "d$000000e0"
    Then expect the next line to contain "d$00000080"
    Then expect the next line to contain "d$00000080"
    Then expect the next line to contain "d$00000080"
    Then expect the next line to contain "d$00000080"
