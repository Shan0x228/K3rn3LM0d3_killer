object MainWindow: TMainWindow
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Kernel mode killer'
  ClientHeight = 344
  ClientWidth = 361
  Color = clBtnFace
  Font.Charset = RUSSIAN_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'Arial'
  Font.Style = [fsBold]
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 16
  object KillProcess_Button: TButton
    Left = 8
    Top = 279
    Width = 145
    Height = 34
    Cursor = crHandPoint
    Caption = 'Kill process'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    OnClick = KillProcess_ButtonClick
  end
  object ProcessContainer_ListBox: TListBox
    Left = 8
    Top = 8
    Width = 337
    Height = 265
    TabOrder = 1
  end
  object UpdateProcess_Button: TButton
    Left = 159
    Top = 279
    Width = 186
    Height = 34
    Cursor = crHandPoint
    Caption = 'Update process list'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
    OnClick = UpdateProcess_ButtonClick
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 325
    Width = 361
    Height = 19
    Panels = <
      item
        Text = 'Driver status:'
        Width = 50
      end>
    ParentShowHint = False
    ShowHint = False
    ExplicitLeft = 560
    ExplicitTop = 280
    ExplicitWidth = 0
  end
end
