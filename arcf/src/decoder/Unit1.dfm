object Form1: TForm1
  Left = 192
  Top = 107
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = #1044#1077#1082#1086#1076#1077#1088' ('#1040#1088#1080#1092#1084#1077#1090#1080#1095#1077#1089#1082#1086#1077' '#1082#1086#1076#1080#1088#1086#1074#1072#1085#1080#1077')'
  ClientHeight = 311
  ClientWidth = 704
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -19
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 24
  object GroupBox1: TGroupBox
    Left = 0
    Top = 8
    Width = 697
    Height = 217
    Caption = #1056#1072#1073#1086#1090#1072' '#1089' '#1092#1072#1081#1083#1072#1084#1080
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    object Label3: TLabel
      Left = 16
      Top = 24
      Width = 210
      Height = 24
      Caption = #1047#1072#1082#1086#1076#1080#1088#1086#1074#1072#1085#1085#1099#1081' '#1092#1072#1081#1083':'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Label4: TLabel
      Left = 16
      Top = 112
      Width = 218
      Height = 24
      Caption = #1042#1086#1089#1089#1090#1072#1085#1086#1074#1083#1077#1085#1085#1099#1081' '#1092#1072#1081#1083':'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Edit2: TEdit
      Left = 8
      Top = 56
      Width = 569
      Height = 32
      AutoSelect = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ReadOnly = True
      TabOrder = 0
    end
    object Edit3: TEdit
      Left = 8
      Top = 152
      Width = 569
      Height = 32
      AutoSelect = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ReadOnly = True
      TabOrder = 1
    end
    object Button3: TButton
      Left = 600
      Top = 56
      Width = 83
      Height = 25
      Caption = #1042#1099#1073#1088#1072#1090#1100
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 2
      OnClick = Button3Click
    end
    object Button4: TButton
      Left = 600
      Top = 152
      Width = 83
      Height = 25
      Caption = #1042#1099#1073#1088#1072#1090#1100
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 3
      OnClick = Button4Click
    end
  end
  object GroupBox2: TGroupBox
    Left = 0
    Top = 232
    Width = 697
    Height = 73
    Caption = #1050#1086#1076#1080#1088#1086#1074#1072#1085#1080#1077
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    object Label1: TLabel
      Left = 208
      Top = 24
      Width = 134
      Height = 24
      Caption = #1042#1088#1077#1084#1077' '#1088#1072#1073#1086#1090#1099':'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Label2: TLabel
      Left = 488
      Top = 24
      Width = 62
      Height = 24
      Caption = #1089#1077#1082#1091#1085#1076
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Button1: TButton
      Left = 8
      Top = 24
      Width = 169
      Height = 33
      Caption = #1056#1072#1079#1072#1088#1093#1080#1074#1080#1088#1086#1074#1072#1090#1100
      Enabled = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      OnClick = Button1Click
    end
    object Button2: TButton
      Left = 600
      Top = 16
      Width = 83
      Height = 25
      Caption = #1042#1099#1093#1086#1076
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
      OnClick = Button2Click
    end
    object Edit1: TEdit
      Left = 352
      Top = 24
      Width = 129
      Height = 32
      AutoSelect = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ReadOnly = True
      TabOrder = 2
    end
  end
  object OpenDialog1: TOpenDialog
    Filter = #1047#1072#1072#1088#1093#1080#1074#1080#1088#1086#1074#1072#1085#1085#1099#1081' '#1092#1072#1081#1083'|*.arf'
    Left = 488
    Top = 24
  end
  object SaveDialog1: TSaveDialog
    Filter = #1042#1089#1077
    Left = 536
    Top = 24
  end
end
