Feature: Tests SimpleCPU design

  Tests the SimpleCPU design by validating files output by the simulation.
  More info: https://docs.google.com/document/d/1scCYfTEw1mQYNbtYTEA1E8sRUwHvc7d2FfL6wf29QfI/edit?usp=sharing


  Scenario: Validates expected memory accesses CPU writes

    Given I run the command line: ..\C64\acme.exe -v -f plain -o target/i0.bin --setpc 0 SimpleCPU_Fibonacci.a
    Then property "test.BDD6502.lastProcessOutput" must contain string "Saving"
    Given I run the command line: ..\C64\acme.exe -v -f plain -o target/i1.bin --setpc 1 SimpleCPU_Fibonacci.a
    Then property "test.BDD6502.lastProcessOutput" must contain string "Saving"
    Given I run the command line: ..\C64\acme.exe -v4 -f plain -o target/i2.bin --setpc 2 SimpleCPU_Fibonacci.a
    Then property "test.BDD6502.lastProcessOutput" must contain string "Saving"


    # The last two writes should occur at: ;@time:0.000184 and ;@time:0.000203
    # So setup the exit to happen just after this (when the last signals will change and trigger the exit test)
    Given I create file "ModelConfig_VSMDD5.txt" with
    """
    EXITPROCCESSAFTER=0.000204
    DELETETHISFILE=1
    """

    Given starting an automation process "cmd" with parameters: /c SimpleCPU.pdsprj
    When automation find window from pattern ".*SimpleCPU.*Proteus.*"
    When automation focus window
    When automation expand main menu item "Debug"
    When automation click current menu item "Run Simulation.*F12"
    Then automation wait for window close
    

    Given open file "output\DebugCPUOutputInternal.txt" for reading
    And skip line
    And skip line
    # Expect 1 written to address 13
    Then expect the next line to contain "d$00000d01"
    And skip line
    And skip line
    # Expect 2 written to address 15
    Then expect the next line to contain "d$00000f02"
    And skip line
    And skip line
    # Expect 3 written to address 17
    Then expect the next line to contain "d$00001103"
    And skip line
    And skip line
    # Expect 5 written to address 19
    Then expect the next line to contain "d$00001305"
    And skip line
    And skip line
    # Expect 8 written to address 21
    Then expect the next line to contain "d$00001508"
    And skip line
    And skip line
    # Expect 13 written to address 23
    Then expect the next line to contain "d$0000170d"
    And skip line
    And skip line
    # Expect 21 written to address 25
    Then expect the next line to contain "d$00001915"
    And skip line
    And skip line
    # Expect 34 written to address 27
    Then expect the next line to contain "d$00001b22"
    And skip line
    And skip line
    # Expect 55 written to address 29
    Then expect the next line to contain "d$00001d37"
    And skip line
    And skip line
    # Expect 89 written to address 31
    Then expect the next line to contain "d$00001f59"
    And skip line
    And skip line
    # Expect 144 written to address 33
    Then expect the next line to contain "d$00002190"
