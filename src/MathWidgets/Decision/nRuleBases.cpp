#include <mathwidgets.h>

N::RuleBases:: RuleBases  ( QWidget * parent , Plan * p             )
             : TreeWidget (           parent ,        p             )
             , Ownership  ( 0 , Types::None , Groups::Subordination )
             , GroupItems (                           p             )
             , RuleType   ( 0                                       )
             , Sorting    ( Qt::AscendingOrder                      )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::RuleBases::~RuleBases(void)
{
}

QSize N::RuleBases::sizeHint(void) const
{
  return QSize ( 320 , 320 ) ;
}

bool N::RuleBases::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true            ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Insert       , Insert      ( ) ) ;
//  LinkAction        ( Rename       , Rename      ( ) ) ;
//  LinkAction        ( Copy         , Copy        ( ) ) ;
  LinkAction        ( Refresh      , startup     ( ) ) ;
  LinkAction        ( Paste        , Paste       ( ) ) ;
//  LinkAction        ( Export       , Export      ( ) ) ;
  LinkAction        ( SelectNone   , SelectNone  ( ) ) ;
  LinkAction        ( SelectAll    , SelectAll   ( ) ) ;
  LinkAction        ( Language     , Language    ( ) ) ;
  LinkAction        ( Font         , setFont     ( ) ) ;
  if ( ! isStandby  (                              ) ) {
    LinkAction      ( Delete       , Delete      ( ) ) ;
  }                                                    ;
  return true                                           ;
}

void N::RuleBases::Configure(void)
{
  NewTreeWidgetItem            ( head                   )              ;
  //////////////////////////////////////////////////////////////////////
  head -> setText              ( 0 , tr("Name")         )              ;
  head -> setText              ( 1 , tr("Type")         )              ;
  head -> setText              ( 2 , ""                 )              ;
  setWindowTitle               ( tr("Rule bases")       )              ;
  //////////////////////////////////////////////////////////////////////
  setFocusPolicy               ( Qt::WheelFocus         )              ;
  setDragDropMode              ( NoDragDrop             )              ;
  setRootIsDecorated           ( false                  )              ;
  setAlternatingRowColors      ( true                   )              ;
  setSelectionMode             ( ExtendedSelection      )              ;
  setColumnCount               ( 3                      )              ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded  )              ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded  )              ;
  //////////////////////////////////////////////////////////////////////
  setAllAlignments             ( head , Qt::AlignCenter )              ;
  setFont                      ( head , Fonts::ListView )              ;
  assignHeaderItems            ( head                   )              ;
  //////////////////////////////////////////////////////////////////////
  MountClicked                 ( 2                      )              ;
  plan -> setFont              ( this                   )              ;
  PassDragDrop = false                                                 ;
  //////////////////////////////////////////////////////////////////////
  RuleNames [ 0                  ] = tr("Undecided"                  ) ;
  RuleNames [ Knowledge::Scope   ] = tr("Value scope"                ) ;
  RuleNames [ Knowledge::Logic   ] = tr("Classical logic rule system") ;
  RuleNames [ Knowledge::Fuzzy   ] = tr("Fuzzy inference system"     ) ;
  RuleNames [ Knowledge::Ann     ] = tr("Artificial Neural Network"  ) ;
  RuleNames [ Knowledge::Game    ] = tr("Game theory rule system"    ) ;
  RuleNames [ Knowledge::Lexical ] = tr("Lexical rule system"        ) ;
}

UUIDs N::RuleBases::Rules(SqlConnection & SC)
{
  QString Q                                           ;
  UUIDs   Uuids                                       ;
  /////////////////////////////////////////////////////
  if ( isStandby ( ) )                                {
    Uuids = SC . Uuids                                (
              PlanTable(Rules)                        ,
              "uuid"                                  ,
              SC.OrderBy   ( "id"       , Sorting ) ) ;
  } else                                              {
    Uuids = Subordination                             (
              SC                                      ,
              ObjectUuid ( )                          ,
              ObjectType ( )                          ,
              Types::RuleBase                         ,
              Connexion  ( )                          ,
              SC . OrderBy ( "position" , Sorting ) ) ;
  }                                                   ;
  /////////////////////////////////////////////////////
  if ( RuleType <= 0 ) return Uuids                   ;
  /////////////////////////////////////////////////////
  SUID  uuid                                          ;
  UUIDs RUIDs                                         ;
  int   rt                                            ;
  /////////////////////////////////////////////////////
  foreach ( uuid ,Uuids)                              {
    rt = 0                                            ;
    Q = SC . sql . SelectFrom                         (
        "type"                                        ,
        PlanTable(Rules)                              ,
        SC . WhereUuid ( uuid )                     ) ;
    if ( SC . Fetch ( Q ) )                           {
      rt = SC . Int ( 0 )                             ;
      if ( rt == RuleType ) RUIDs << uuid             ;
    }                                                 ;
  }                                                   ;
  /////////////////////////////////////////////////////
  Uuids = RUIDs                                       ;
  /////////////////////////////////////////////////////
  return Uuids                                        ;
}

void N::RuleBases::run(int Type,ThreadData * data)
{
  switch ( Type ) {
    case 10001    :
      List ( )    ;
    break         ;
  }               ;
}

bool N::RuleBases::startup(void)
{
  clear (       ) ;
  start ( 10001 ) ;
  return true     ;
}

bool N::RuleBases::List(void)
{
  if ( RuleType > 0 )                                 {
    setColumnHidden       ( 1 , true                ) ;
    setWindowTitle        ( RuleNames[RuleType]     ) ;
  }                                                   ;
  /////////////////////////////////////////////////////
  EnterSQL                ( SC , plan->sql          ) ;
    QString Q                                         ;
    UUIDs   Uuids = Rules ( SC )                      ;
    SUID    uuid                                      ;
    foreach ( uuid , Uuids )                          {
      QString name                                    ;
      int     type                                    ;
      name = SC . getName                             (
               PlanTable(Names)                       ,
               "uuid"                                 ,
               vLanguageId                            ,
               uuid                                 ) ;
      Q = SC . sql . SelectFrom                       (
            "type"                                    ,
            PlanTable(Rules)                          ,
            SC . WhereUuid ( uuid )                 ) ;
      if ( SC . Fetch ( Q ) )                         {
        QString rn                                    ;
        type = SC . Int   ( 0                       ) ;
        rn   = RuleNames  [ type                    ] ;
        NewTreeWidgetItem ( item                    ) ;
        item -> setData   ( 0 , Qt::UserRole , uuid ) ;
        item -> setData   ( 1 , Qt::UserRole , type ) ;
        item -> setText   ( 0 , name                ) ;
        item -> setText   ( 1 , rn                  ) ;
        addTopLevelItem   ( item                    ) ;
      }                                               ;
    }                                                 ;
  ErrorSQL                ( SC , plan->sql          ) ;
  LeaveSQL                ( SC , plan->sql          ) ;
  /////////////////////////////////////////////////////
  emit AutoFit            (                         ) ;
  Alert                   ( Done                    ) ;
  return true                                         ;
}

bool N::RuleBases::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )              ;
  QTreeWidgetItem * item = itemAt(pos)   ;
  QAction    * aa                        ;
  if (NotNull(item))                     {
    mm.add(101,tr("Edit"  ))             ;
  }                                      ;
  mm . add(102,tr("Insert"))             ;
  if (ObjectUuid()>0 && NotNull(item))   {
    mm.add(103,tr("Delete"))             ;
  }                                      ;
  mm . add(201,tr("Refresh"))            ;
  mm . setFont(plan)                     ;
  aa = mm . exec ( )                     ;
  if (IsNull(aa)) return true            ;
  switch (mm[aa])                        {
    case 101                             :
      emit Edit ( nTreeUuid ( item,0 )   ,
                  nTreeInt  ( item,1 ) ) ;
    break                                ;
    case 102                             :
      Insert  ( )                        ;
    break                                ;
    case 103                             :
      Delete  ( )                        ;
    break                                ;
    case 201                             :
      startup ( )                        ;
    break                                ;
  }                                      ;
  return true                            ;
}

SUID N::RuleBases::assureItem (
        SqlConnection & SC    ,
        SUID            u     ,
        QString         name  )
{
  if ( u <= 0 )                                                             {
    u  = SC . Unique ( PlanTable(MajorUuid) , "uuid" , 8968               ) ;
    SC . assureUuid  ( PlanTable(MajorUuid) , u      , Types::RuleBase    ) ;
    SC . assureUuid  ( PlanTable(Rules)     , u      , RuleType           ) ;
    if ( ( u > 0 ) && ( ! isStandby ( ) ) )                                 {
      GroupItems::Join                                                      (
        SC                                                                  ,
        ObjectUuid ( )                                                      ,
        ObjectType ( )                                                      ,
        Types::RuleBase                                                     ,
        Connexion  ( )                                                      ,
        0                                                                   ,
        u                                                                 ) ;
    }                                                                       ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  if ( u > 0 )                                                              {
    SC . assureName  ( PlanTable(Names)     , u      , vLanguageId , name ) ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  return u                                                                  ;
}

void N::RuleBases::Paste(void)
{
  Paste ( nClipboardText ) ;
}

void N::RuleBases::Paste(QString text)
{
  QStringList L = text . split ( '\n'                         ) ;
  if ( L . count ( ) <= 0 ) return                              ;
  EnterSQL                     ( SC , plan->sql               ) ;
    QString n                                                   ;
    SUID    u                                                   ;
    foreach                    ( n  , L                       ) {
      NewTreeWidgetItem        ( it                           ) ;
      u     = assureItem       ( SC , 0            , n        ) ;
      it   -> setData          ( 0  , Qt::UserRole , u        ) ;
      it   -> setData          ( 1  , Qt::UserRole , RuleType ) ;
      it   -> setText          ( 0  , n                       ) ;
      it   -> setText          ( 1  , RuleNames[RuleType]     ) ;
      addTopLevelItem          ( it                           ) ;
      plan -> processEvents    (                              ) ;
    }                                                           ;
  ErrorSQL                     ( SC , plan->sql               ) ;
  LeaveSQL                     ( SC , plan->sql               ) ;
  Alert                        ( Done                         ) ;
}

void N::RuleBases::Insert(void)
{
  NewTreeWidgetItem ( IT                   ) ;
  IT -> setData     ( 0 , Qt::UserRole , 0 ) ;
  IT -> setData     ( 1 , Qt::UserRole , 0 ) ;
  setAlignments     ( IT                   ) ;
  addTopLevelItem   ( IT                   ) ;
  scrollToItem      ( IT                   ) ;
  doubleClicked     ( IT , 0               ) ;
}

void N::RuleBases::Delete(void)
{
  nDropOut ( isStandby ( )    ) ;
  QTreeWidgetItem * item        ;
  item = currentItem()          ;
  nDropOut ( IsNull(item)     ) ;
  SUID  uuid                    ;
  uuid = nTreeUuid(item,0)      ;
  nDropOut ( uuid <= 0        ) ;
  EnterSQL ( SC , plan->sql   ) ;
    UUIDs RUIDs                 ;
    RUIDs << uuid               ;
    GroupItems :: Detach        (
      SC                        ,
      ObjectUuid ( )            ,
      ObjectType ( )            ,
      Types::RuleBase           ,
      Connexion  ( )            ,
      RUIDs                   ) ;
  LeaveSQL ( SC , plan->sql   ) ;
  takeItem ( item             ) ;
  Alert    ( Done             ) ;
}

void N::RuleBases::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * line                         ;
  QComboBox * cc                           ;
  int         type = nTreeInt (item,1)     ;
  switch (column)                          {
    case 0                                 :
      line = setLineEdit                   (
               item,column                 ,
               SIGNAL(editingFinished())   ,
               SLOT  (nameFinished   ()) ) ;
      line->setFocus( Qt::TabFocusReason ) ;
    break                                  ;
    case 1                                 :
      cc   = setComboBox                   (
               item,column                 ,
               SIGNAL(activated(int))      ,
               SLOT(typeFinished(int))   ) ;
      N::AddItems(*cc,RuleNames)           ;
      (*cc) <= type                        ;
      cc->setEditable(true)                ;
      cc->showPopup()                      ;
    break                                  ;
  }                                        ;
}

void N::RuleBases::nameFinished(void)
{
  if (IsNull(ItemEditing)) return                             ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget)            ;
  if (IsNull(line)) return                                    ;
  SUID    uuid = nTreeUuid(ItemEditing,0)                     ;
  QString name = line->text()                                 ;
  /////////////////////////////////////////////////////////////
  EnterSQL                   ( SC , plan->sql               ) ;
    if ( ( uuid <= 0 ) && ( name . length ( ) > 0 ) )         {
      QString rt = RuleNames [ RuleType                     ] ;
      uuid = assureItem      ( SC , uuid         , name     ) ;
      ItemEditing -> setData ( 0  , Qt::UserRole , uuid     ) ;
      ItemEditing -> setData ( 1  , Qt::UserRole , RuleType ) ;
      ItemEditing -> setText ( 1  , rt                      ) ;
    }                                                         ;
    if ( uuid > 0 )                                           {
      ItemEditing -> setText ( 0  , name                    ) ;
    }                                                         ;
  ErrorSQL                   ( SC , plan->sql               ) ;
  LeaveSQL                   ( SC , plan->sql               ) ;
  /////////////////////////////////////////////////////////////
  removeOldItem              ( true , 0                     ) ;
  Alert                      ( Done                         ) ;
}

void N::RuleBases::typeFinished(int)
{
  if ( IsNull(ItemEditing) ) return                      ;
  QComboBox * cc = Casting ( QComboBox , ItemWidget )    ;
  if ( IsNull(cc         ) ) return                      ;
  ////////////////////////////////////////////////////////
  SUID uuid = nTreeUuid ( ItemEditing , 0 )              ;
  int  type = (int) N::GetUuid ( cc )                    ;
  ////////////////////////////////////////////////////////
  EnterSQL         ( SC , plan->sql                    ) ;
    QString Q                                            ;
    if ( uuid > 0 )                                      {
      Q = SC . sql . Update                              (
            PlanTable(Rules)                             ,
            "where uuid = :UUID"                         ,
            1                                            ,
            "type"                                     ) ;
      SC . Prepare ( Q                                 ) ;
      SC . Bind    ( "uuid" , uuid                     ) ;
      SC . Bind    ( "type" , type                     ) ;
      SC . Exec    (                                   ) ;
      ItemEditing -> setData ( 1 , Qt::UserRole , type ) ;
      ItemEditing -> setText ( 1 , cc->currentText()   ) ;
    }                                                    ;
  LeaveSQL         ( SC , plan->sql                    ) ;
  removeOldItem    ( true , 0                          ) ;
  Alert            ( Done                              ) ;
}
