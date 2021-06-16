#include <mathwidgets.h>

N::SetsRules:: SetsRules ( QWidget * parent , Plan * p  )
             : TreeDock  (           parent ,        p  )
             , Object    ( 0 , Types::None              )
             , Relation  ( N::Groups::Subordination , 0 )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SetsRules::~SetsRules(void)
{
}

void N::SetsRules::Configure(void)
{
  NewTreeWidgetItem        (head                                ) ;
  /////////////////////////////////////////////////////////////////
  setWindowTitle           ( tr("Sets algebra rulesets")        ) ;
  setAccessibleName        ( "nSetsRules"                       ) ;
  /////////////////////////////////////////////////////////////////
  setColumnCount           ( 3                                  ) ;
  setRootIsDecorated       ( false                              ) ;
  setAlternatingRowColors  ( true                               ) ;
  setSelectionMode         ( ExtendedSelection                  ) ;
  /////////////////////////////////////////////////////////////////
  head -> setText          ( 0 , tr("Name")                     ) ;
  head -> setText          ( 1 , tr("Identifier")               ) ;
  head -> setText          ( 2 , tr("Sets algebra rule syntax") ) ;
  setAllAlignments         ( head , Qt::AlignCenter             ) ;
  setFont                  ( head , Fonts::ListView             ) ;
  setHeaderItem            ( head                               ) ;
  /////////////////////////////////////////////////////////////////
  plan -> setFont          ( this                               ) ;
  setDragDropMode          ( DragDrop                           ) ;
  MountClicked             ( 2                                  ) ;
}

bool N::SetsRules::FocusIn(void)
{
  AssignAction ( Label   , windowTitle ( ) )           ;
  LinkAction   ( Refresh , startup     ( ) )           ;
  LinkAction   ( Insert  , Insert      ( ) )           ;
  if (ObjectUuid()>0 && ObjectType()>0 && relation>0)  {
    LinkAction ( Delete  , Delete      ( ) )           ;
  }                                                    ;
  return true                                          ;
}

bool N::SetsRules::hasItem(void)
{
  QTreeWidgetItem * item = currentItem () ;
  return NotNull ( item )                 ;
}

bool N::SetsRules::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())               ;
  nKickOut (  IsNull(atItem)                          , false ) ;
  nKickOut ( !IsMask(event->buttons(),Qt::LeftButton) , false ) ;
  dragPoint = event->pos()                                      ;
  nKickOut ( !atItem->isSelected()                    , false ) ;
  nKickOut ( !PassDragDrop                            , false ) ;
  return true                                                   ;
}

bool N::SetsRules::fetchDrag(QMouseEvent * event)
{
  nKickOut ( !IsMask(event->buttons(),Qt::LeftButton) , false ) ;
  QPoint pos = event->pos()                                     ;
  pos -= dragPoint                                              ;
  return ( pos.manhattanLength() > qApp->startDragDistance() )  ;
}

QMimeData * N::SetsRules::dragMime(void)
{
  return standardMime("setsalgebra") ;
}

void N::SetsRules::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::SetsRules::finishDrag(QMouseEvent * event)
{
  return true ;
}

bool N::SetsRules::acceptDrop(QWidget * source,const QMimeData * mime)
{
  if (source==this) return false ;
  return dropHandler ( mime )    ;
}

bool N::SetsRules::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  return true ;
}

bool N::SetsRules::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  return true ;
}

bool N::SetsRules::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (source==this) return false           ;
  return dropItems ( source , mime , pos ) ;
}

QString N::SetsRules::MimeType(const QMimeData * mime)
{
  return AbstractGui::MimeType(mime,"setsalgebra/uuid;setsalgebra/uuids") ;
}

UUIDs N::SetsRules::MimeUuids(const QMimeData * mime,QString mimetype)
{
  UUIDs Uuids                            ;
  QByteArray data = mime->data(mimetype) ;
  if (data.size()<=0) return Uuids       ;
  if (mimetype=="setsalgebra/uuid")      {
    SUID * suid = (SUID *)data.data()    ;
    Uuids << (*suid)                     ;
  } else
  if (mimetype=="setsalgebra/uuids")     {
    SUID * suid  = (SUID *)data.data()   ;
    int    total = (int)suid[0]          ;
    for (int i=0;i<total;i++)            {
      Uuids << suid[i+1]                 ;
      DoProcessEvents                    ;
    }                                    ;
  }                                      ;
  return Uuids                           ;
}

void N::SetsRules::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * le = NULL                      ;
  switch (column)                            {
    case 0                                   :
      le  = setLineEdit                      (
              item                           ,
              column                         ,
              SIGNAL(editingFinished())      ,
              SLOT  (nameFinished   ())    ) ;
      le -> setFocus ( Qt::TabFocusReason  ) ;
    break                                    ;
    case 1                                   :
      le  = setLineEdit                      (
              item                           ,
              column                         ,
              SIGNAL(editingFinished   ())   ,
              SLOT  (identifierFinished()) ) ;
      le -> setFocus ( Qt::TabFocusReason  ) ;
    break                                    ;
    case 2                                   :
      le  = setLineEdit                      (
              item                           ,
              column                         ,
              SIGNAL(editingFinished())      ,
              SLOT  (ruleFinished   ())    ) ;
      le -> setFocus ( Qt::TabFocusReason  ) ;
    break                                    ;
    default                                  :
      removeOldItem ( )                      ;
    break                                    ;
  }                                          ;
}

void N::SetsRules::run(int T,ThreadData * d)
{
  nDropOut         ( ! IsContinue ( d ) ) ;
  switch           ( T                  ) {
    case 10001                            :
      startLoading (                    ) ;
      Load         ( d                  ) ;
      stopLoading  (                    ) ;
    break                                 ;
  }                                       ;
}

bool N::SetsRules::Load(ThreadData * d)
{
  nKickOut ( ! IsContinue ( d ) , false ) ;
  return true                             ;
}

bool N::SetsRules::startup(void)
{
  clear          (                )          ;
  GroupItems GI  ( plan           )          ;
  EnterSQL       ( SC , plan->sql )          ;
    QString Q                                ;
    QString name                             ;
    QString ident                            ;
    QString op                               ;
    UUIDs   Uuids                            ;
    SUID    uu                               ;
    if (ObjectUuid()>0 && ObjectType()>0)    {
      Uuids = GI.Subordination               (
                SC                           ,
                ObjectUuid()                 ,
                ObjectType()                 ,
                N::Types::SetsAlgebra        ,
                relation                     ,
                SC.OrderByAsc("position")  ) ;
    } else                                   {
      Uuids = SC.Uuids                       (
                PlanTable(SetsAlgebra)       ,
                "uuid"                       ,
                SC.OrderByAsc("id")        ) ;
    }                                        ;
    foreach (uu,Uuids)                       {
      NewTreeWidgetItem(it)                  ;
      name  = GetName(SC,uu)                 ;
      ident = ""                             ;
      Q = SC.sql.SelectFrom                  (
            "name"                           ,
            PlanTable(Identifiers)           ,
            SC.WhereUuid(uu)               ) ;
      if (SC.Fetch(Q)) ident = SC.String(0)  ;
      Q = SC.sql.SelectFrom                  (
            "operation"                      ,
            PlanTable(SetsAlgebra)           ,
            SC.WhereUuid(uu)               ) ;
      if (SC.Fetch(Q)) op = SC.String(0)     ;
      it->setText(0,name                   ) ;
      it->setText(1,ident                  ) ;
      it->setText(2,op                     ) ;
      it->setData(0,Qt::UserRole,uu        ) ;
      setAlignments  (it)                    ;
      addTopLevelItem(it)                    ;
    }                                        ;
  LeaveSQL        ( SC , plan->sql )         ;
  SuitableColumns (                )         ;
  Alert           ( Done           )         ;
  return true                                ;
}

void N::SetsRules::Insert(void)
{
  removeOldItem     (                       ) ;
  NewTreeWidgetItem ( IT                    ) ;
  addTopLevelItem   ( IT                    ) ;
  scrollToItem      ( IT                    ) ;
  IT->setData       ( 0  , Qt::UserRole , 0 ) ;
  doubleClicked     ( IT , 0                ) ;
}

void N::SetsRules::nameFinished(void)
{
  SUID        uu   = nTreeUuid ( ItemEditing,0          )   ;
  QLineEdit * name = Casting   ( QLineEdit  ,ItemWidget )   ;
  ///////////////////////////////////////////////////////////
  if (IsNull(name))                                         {
    removeOldItem ( )                                       ;
    return                                                  ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  QString cn = name->text()                                 ;
  if (cn.length()>0)                                        {
    EnterSQL ( SC , plan->sql )                             ;
      QString Q                                             ;
      if (uu<=0)                                            {
        uu = SC.Unique(PlanTable(MajorUuid),"uuid",6633)    ;
        SC.insertUuid                                       (
          PlanTable(MajorUuid)                              ,
          uu                                                ,
          N::Types::SetsAlgebra                           ) ;
        SC.insertUuid(PlanTable(SetsAlgebra),uu,"uuid")     ;
        if (ObjectUuid()>0 && ObjectType()>0 && relation>0) {
          GroupItems GI(plan)                               ;
          UUIDs Uuids                                       ;
          Uuids << uu                                       ;
          GI.Join                                           (
            SC                                              ,
            ObjectUuid()                                    ,
            ObjectType()                                    ,
            N::Types::SetsAlgebra                           ,
            relation                                        ,
            0                                               ,
            Uuids                                         ) ;
        }                                                   ;
        ItemEditing->setData(0,Qt::UserRole,uu)             ;
      }                                                     ;
      SC.assureName(PlanTable(Names),uuid,vLanguageId,cn)   ;
      ItemEditing->setText(0,cn               )             ;
    LeaveSQL ( SC , plan->sql )                             ;
  }                                                         ;
  removeOldItem (      )                                    ;
  Alert         ( Done )                                    ;
}

void N::SetsRules::identifierFinished(void)
{
  SUID        uu   = nTreeUuid ( ItemEditing,0          ) ;
  QLineEdit * name = Casting   ( QLineEdit  ,ItemWidget ) ;
  /////////////////////////////////////////////////////////
  if (IsNull(name))                                       {
    removeOldItem ( )                                     ;
    return                                                ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  QString cn = name->text()                               ;
  if (cn.length()>0)                                      {
    FinanceManager FM(plan)                               ;
    EnterSQL ( SC , plan->sql )                           ;
      FM.assureIdentifier(SC,uu,cn)                       ;
      ItemEditing->setText(1,cn)                          ;
    LeaveSQL ( SC , plan->sql )                           ;
  }                                                       ;
  removeOldItem (      )                                  ;
  Alert         ( Done )                                  ;
}

void N::SetsRules::ruleFinished(void)
{
  SUID        uu   = nTreeUuid ( ItemEditing,0          )  ;
  QLineEdit * name = Casting   ( QLineEdit  ,ItemWidget )  ;
  //////////////////////////////////////////////////////////
  if (IsNull(name))                                        {
    removeOldItem ( )                                      ;
    return                                                 ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  QString cn = name->text()                                ;
  if (cn.length()>0)                                       {
    SetsAlgebra NSA                                        ;
    if (NSA.Decode(cn))                                    {
      EnterSQL ( SC , plan->sql )                          ;
        QString Q                                          ;
        Q = SC.sql.Update                                  (
              PlanTable(SetsAlgebra)                       ,
              SC.sql.Where(1,"uuid")                       ,
              1                                            ,
              "operation"                                ) ;
        SC . Prepare ( Q                         )         ;
        SC . Bind    ( "uuid"      , uu          )         ;
        SC . Bind    ( "operation" , cn.toUtf8() )         ;
        SC . Exec    (                           )         ;
        ItemEditing->setText(2,cn)                         ;
      LeaveSQL ( SC , plan->sql )                          ;
      Alert         ( Done )                               ;
    } else                                                 {
      Notify(tr("Sets syntax error"),cn)                   ;
      Alert         ( Error )                              ;
    }                                                      ;
  }                                                        ;
  removeOldItem (      )                                   ;
}

bool N::SetsRules::Menu(QPoint pos)
{
  return true ;
}
