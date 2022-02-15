Feature: Tests SimpleCPU design

  Tests the SimpleCPU design by validating files output by the simulation.
  More info: https://docs.google.com/document/d/1scCYfTEw1mQYNbtYTEA1E8sRUwHvc7d2FfL6wf29QfI/edit?usp=sharing


  Scenario: Validates expected memory accesses CPU writes
    Given open file "output\DebugCPUOutputInternal.txt" for reading
    And skip line
    # Expect 1 written to address 13
    Then expect the next line to contain "d$00000d01"
    And skip line
    # Expect 2 written to address 15
    Then expect the next line to contain "d$00000f02"
    And skip line
    # Expect 3 written to address 17
    Then expect the next line to contain "d$00001103"
    And skip line
    # Expect 5 written to address 19
    Then expect the next line to contain "d$00001305"
    And skip line
    # Expect 8 written to address 21
    Then expect the next line to contain "d$00001508"
    And skip line
    # Expect 13 written to address 23
    Then expect the next line to contain "d$0000170d"
    And skip line
    # Expect 21 written to address 25
    Then expect the next line to contain "d$00001915"
    And skip line
    # Expect 34 written to address 27
    Then expect the next line to contain "d$00001b22"
    And skip line
    # Expect 55 written to address 29
    Then expect the next line to contain "d$00001d37"
    And skip line
    # Expect 89 written to address 31
    Then expect the next line to contain "d$00001f59"
    And skip line
    # Expect 144 written to address 33
    Then expect the next line to contain "d$00002190"
