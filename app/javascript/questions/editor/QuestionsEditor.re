[@bs.config {jsx: 3}];

let str = React.string;

open QuestionsShow__Types;

module CreateQuestionQuery = [%graphql
  {|
  mutation($title: String!, $description: String!, $communityId: ID!, $targetId: ID) {
    createQuestion(description: $description, title: $title, communityId: $communityId, targetId: $targetId) @bsVariant {
      questionId
      errors
    }
  }
|}
];

module UpdateQuestionQuery = [%graphql
  {|
  mutation($id: ID!, $title: String!, $description: String!) {
    updateQuestion(id: $id, title: $title, description: $description) @bsVariant {
      success
      errors
    }
  }
|}
];

module CreateQuestionError = {
  type t = [
    | `InvalidLengthTitle
    | `InvalidLengthDescription
    | `BlankCommunityID
  ];

  let notification = error =>
    switch (error) {
    | `InvalidLengthTitle => (
        "InvalidLengthTitle",
        "Supplied title must be between 1 and 250 characters in length",
      )
    | `InvalidLengthDescription => (
        "InvalidLengthDescription",
        "Supplied description must be greater than 1 characters in length",
      )
    | `BlankCommunityID => (
        "BlankCommunityID",
        "Community id is required for creating Answer",
      )
    };
};

module UpdateQuestionError = {
  type t = [ | `InvalidLengthTitle | `InvalidLengthDescription];

  let notification = error =>
    switch (error) {
    | `InvalidLengthTitle => (
        "InvalidLengthTitle",
        "Supplied title must be between 1 and 250 characters in length",
      )
    | `InvalidLengthDescription => (
        "InvalidLengthDescription",
        "Supplied description must be greater than 1 characters in length",
      )
    };
};

let handleResponseCB = (id, title) => {
  let window = Webapi.Dom.window;
  let parameterizedTitle =
    title
    |> Js.String.toLowerCase
    |> Js.String.replaceByRe([%re "/[^0-9a-zA-Z]+/gi"], "-");
  let redirectPath = "/questions/" ++ id ++ "/" ++ parameterizedTitle;
  redirectPath |> Webapi.Dom.Window.setLocation(window);
};

module CreateQuestionErrorHandler =
  GraphqlErrorHandler.Make(CreateQuestionError);

module UpdateQuestionErrorHandler =
  GraphqlErrorHandler.Make(UpdateQuestionError);

let handleCreateOrUpdateQuestion =
    (
      title,
      description,
      communityId,
      authenticityToken,
      setSaving,
      target,
      question,
      updateQuestionCB,
      event,
    ) => {
  event |> ReactEvent.Mouse.preventDefault;

  if (description != "") {
    setSaving(_ => true);
    switch (question) {
    | Some(question) =>
      let id = question |> Question.id;
      UpdateQuestionQuery.make(~id, ~title, ~description, ())
      |> GraphqlQuery.sendQuery(authenticityToken)
      |> Js.Promise.then_(response =>
           switch (response##updateQuestion) {
           | `Success(updated) =>
             switch (updated, updateQuestionCB) {
             | (true, Some(questionCB)) =>
               questionCB(title, description);
               Notification.success("Done!", "Question Updated sucessfully");
             | (_, _) =>
               Notification.error(
                 "Something went wrong",
                 "Please refresh the page and try again",
               )
             };
             Js.Promise.resolve();
           | `Errors(errors) =>
             Js.Promise.reject(UpdateQuestionErrorHandler.Errors(errors))
           }
         )
      |> UpdateQuestionErrorHandler.catch(() => setSaving(_ => false))
      |> ignore;
    | None =>
      (
        switch (target) {
        | Some(target) =>
          CreateQuestionQuery.make(
            ~description,
            ~title,
            ~communityId,
            ~targetId=target |> QuestionsEditor__Target.id,
            (),
          )
        | None =>
          CreateQuestionQuery.make(~description, ~title, ~communityId, ())
        }
      )
      |> GraphqlQuery.sendQuery(authenticityToken)
      |> Js.Promise.then_(response =>
           switch (response##createQuestion) {
           | `QuestionId(questionId) =>
             handleResponseCB(questionId, title);
             Notification.success("Done!", "Question has been saved.");
             Js.Promise.resolve();
           | `Errors(errors) =>
             Js.Promise.reject(CreateQuestionErrorHandler.Errors(errors))
           }
         )
      |> CreateQuestionErrorHandler.catch(() => setSaving(_ => false))
      |> ignore
    };
  } else {
    Notification.error("Empty", "Answer cant be blank");
  };
};

[@react.component]
let make =
    (
      ~authenticityToken,
      ~communityId,
      ~communityPath=?,
      ~target,
      ~question=?,
      ~updateQuestionCB=?,
    ) => {
  let (saving, setSaving) = React.useState(() => false);
  let (description, setDescription) =
    React.useState(() =>
      switch (question) {
      | Some(q) => q |> Question.description
      | None => ""
      }
    );
  let (title, setTitle) =
    React.useState(() =>
      switch (question) {
      | Some(q) => q |> Question.title
      | None => ""
      }
    );
  let updateDescriptionCB = description => setDescription(_ => description);
  let saveDisabled = description == "" || title == "";

  <div className="bg-gray-100">
    <div className="flex-1 flex flex-col">
      <div>
        {
          switch (communityPath) {
          | Some(communityPath) =>
            <div className="max-w-3xl w-full mx-auto mt-5 pb-2">
              <a className="btn btn-default" href=communityPath>
                <i className="far fa-arrow-left" />
                <span className="ml-2"> {React.string("Back")} </span>
              </a>
            </div>
          | None => React.null
          }
        }
      </div>
      {
        switch (target) {
        | Some(target) =>
          <div className="max-w-3xl w-full mt-5 mx-auto px-3 md:px-0">
            <div
              className="flex py-4 px-4 md:px-6 w-full bg-yellow-100 border border-dashed border-yellow-400 rounded justify-between items-center">
              <p className="w-3/5 md:w-4/5 font-semibold text-sm">
                {"Target:" ++ (target |> QuestionsEditor__Target.title) |> str}
              </p>
              <a
                href="./new"
                className="no-underline bg-yellow-100 border border-yellow-400 px-3 py-2 hover:bg-yellow-200 rounded-lg cursor-pointer text-xs font-semibold">
                {"Clear" |> str}
              </a>
            </div>
          </div>
        | None => React.null
        }
      }
      <div
        className="mb-8 max-w-3xl w-full mx-auto relative shadow border bg-white rounded-lg">
        <div className="flex w-full flex-col py-4 px-4">
          <h5
            className="uppercase text-center border-b border-gray-400 pb-2 mb-4">
            {
              (
                switch (question) {
                | Some(_) => "Edit Question"
                | None => "Ask a new Question"
                }
              )
              |> str
            }
          </h5>
          <label
            className="inline-block tracking-wide text-gray-700 text-xs font-semibold mb-2"
            htmlFor="title">
            {"Title" |> str}
          </label>
          <input
            id="title"
            value=title
            className="appearance-none block w-full bg-white text-gray-700 border border-gray-400 rounded py-3 px-4 mb-6 leading-tight focus:outline-none focus:bg-white focus:border-gray"
            onChange={
              event => setTitle(ReactEvent.Form.target(event)##value)
            }
          />
          <div className="w-full flex flex-col">
            <DisablingCover disabled=saving>
              <MarkDownEditor
                updateDescriptionCB
                value=description
                placeholder="You can use Markdown to format this text."
                label="Body"
              />
            </DisablingCover>
            <div className="flex justify-end pt-3 border-t">
              <button
                disabled=saveDisabled
                onClick={
                  handleCreateOrUpdateQuestion(
                    title,
                    description,
                    communityId,
                    authenticityToken,
                    setSaving,
                    target,
                    question,
                    updateQuestionCB,
                  )
                }
                className="btn btn-primary">
                {
                  (
                    switch (question) {
                    | Some(_) => "Update Question"
                    | None => "Post Your Question"
                    }
                  )
                  |> str
                }
              </button>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>;
};