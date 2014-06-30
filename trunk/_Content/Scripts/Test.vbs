Option Explicit

MsgBox "Test.vbs"

Dim sProjectId: sProjectId = WScript.Arguments(0)
Dim sName:      sName = WScript.Arguments(1)

Dim properties: Set properties = CreateObject("ProjSrv.rBldProperties.1")

properties.Init(sProjectId & ".Properties")

MsgBox properties.GetProperty("WindowsFolder")
properties.SetProperty "setfromscript", "property set from script"
MsgBox properties.GetProperty("setfromscript")

properties.UnInit
Set properties = Nothing

Dim project: Set project = CreateObject("ProjSrv.rBldProjectXml.1")

project.Init(sProjectId & ".Project")

Dim sQuery: sQuery = "//BuildIt/Actions/Action[@name='" & sName & "']"
project.Select(sQuery)

MsgBox project.GetAttribute("script")
project.SetAttribute "script01", "test setting new value"
MsgBox project.GetAttribute("script01")
MsgBox project.GetXPath()
MsgBox project.GetAttribute("script")
MsgBox project.GetXmlFragment()

project.UnInit
Set project = Nothing

MsgBox "Done"

